#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

#define GROUPSIZE 32
#define MEMSIZE 2*GROUPSIZE

int Index( int Y, int X, int H, int W )
{
    return (Y * W) + X;
}

int IndexTranspose( int Y, int X, int H, int W )
{
    return (X * H) + Y;
}

bool IsInside( int Y, int X, int H, int W )
{
    if( (Y >= 0) && (Y < H) )
    {
        if( (X >= 0) && (X < W) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool IsInsideTranspose( int Y, int X, int H, int W )
{
    if( (Y >= 0) && (Y < H) )
    {
        if( (X >= 0) && (X < W) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool IsInsideMask( int Y, int X, int H, int W, __global const uchar *Mask )
{
    if( IsInside( Y, X, H, W ) )
    {
        if( Mask[ Index( Y, X, H, W ) ] != 0 )
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

__kernel void MeanFilter_WithMask_1(__global const uchar4 *ImIn, __global const uchar *Mask,
	__global uchar4 *ImOut, int Height, int Width, int Radius)
{
	
	// 2 * localsize
	__local int4 LocalData[64];
	__local int LocalMask[64];

	const int GlobalPosY = get_global_id(1);
	const int GlobalPosX = get_global_id(0);
	

	int4 Sum = (int4)(0);
	int8 DSum = (int8)(0);
	int4 Counter = 0;

	for (int j = -Radius; j <= Radius; ++j)
	{
		int row = GlobalPosY + j;
		for (int i = -Radius; i <= Radius; ++i)
		{
			int col = GlobalPosX + i;
			if (IsInside(row, col, Height, Width))
			{
				Sum += convert_int4(ImIn[Index(row, col, Height, Width)]);
				Counter += (int4)(1);
			}
		}
	}

	if (Counter.x > 0)
	{
		DSum.lo = Sum;
		DSum.hi = Counter;
	}

	if (IsInside(GlobalPosY, GlobalPosX, Height, Width))
	{
		ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = convert_uchar4(DSum.lo / DSum.hi);
		//ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)];
	}
}

__kernel void MeanFilter_WithMask_H( __global const uchar4 *ImIn, __global const uchar *Mask,
                                     __global int8 *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int4 LocalData[ 64 ];
    __local int LocalMask[ 64 ];
    
    const int GlobalPosY = get_global_id( 1 );
    const int GlobalPosX = get_global_id( 0 );
    
    const int LocalPosY = get_local_id( 1 );
    const int LocalPosX = get_local_id( 0 );
    
    const int LocalSizeY = get_local_size( 1 );
    const int LocalSizeX = get_local_size( 0 );
    
    int4 Sum = 0;
    int8 DSum = (int8)(0);
    int4 Counter = 0;
    
    int Col = GlobalPosX - Radius;
    
    // First read
    if( IsInsideMask( GlobalPosY, Col, Height, Width, Mask ) )
    {
        LocalData[ LocalPosX ] = convert_int4(ImIn[ Index( GlobalPosY, Col, Height, Width ) ]);
        LocalMask[ LocalPosX ] = 1;
    }
    else
    {
		LocalData[LocalPosX] = (int4)(0);
        LocalMask[ LocalPosX ] = 0;
    }

    for( ; Col <= GlobalPosX + Radius; Col += LocalSizeX )
    {

        // Read the second part
        if( IsInsideMask( GlobalPosY, Col + LocalSizeX, Height, Width, Mask ) )
        {
            LocalData[ LocalPosX + LocalSizeX ] = convert_int4(ImIn[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ]);
            LocalMask[ LocalPosX + LocalSizeX ] = 1;
        }
        else
        {
			LocalData[LocalPosX + LocalSizeX] = (int4)(0);
            LocalMask[ LocalPosX + LocalSizeX ] = 0;
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        for( int j = 0; j < LocalSizeX && (Col + j <= GlobalPosX + Radius); ++j )
        {
            Sum = Sum + LocalData[ LocalPosX + j ];
            Counter = Counter + (int4)(LocalMask[ LocalPosX + j ]);
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        // Copy the second part into the first part of local memory
        LocalData[ LocalPosX ] = LocalData[ LocalPosX + LocalSizeX ];
        LocalMask[ LocalPosX ] = LocalMask[ LocalPosX + LocalSizeX ];
    }
    
	if (Counter.s0 > 0)
	{
		DSum.lo = convert_int4(Sum);
		//DSum.lo = convert_int4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)]);
		DSum.hi = convert_int4(Counter);
	}
        
    if( IsInside( GlobalPosY, GlobalPosX, Height, Width ) )
        ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = DSum;
}

__kernel void MeanFilter_WithMask_V( __global int8 *ImIn, __global const uchar *Mask,
                                     __global uchar4 *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int4 LocalData[ 64 ];
    __local int LocalMask[ 64 ];
    
    const int GlobalPosY = get_global_id( 0 );
    const int GlobalPosX = get_global_id( 1 );
    
    const int LocalPosY = get_local_id( 0 );
    const int LocalPosX = get_local_id( 1 );
    
    const int LocalSizeY = get_local_size( 0 );
    const int LocalSizeX = get_local_size( 1 );
    
    int4 Sum = 0;
    double4 DSum = 0.0;
    int4 Counter = 0;
    int OffsetY = 0;
    int8 Temp;
    int Row = GlobalPosY - Radius;
    
    // First read into local memory
    if( IsInside( Row, GlobalPosX, Height, Width ) )
    {
        Temp = ImIn[ Index( Row, GlobalPosX, Height, Width ) ];
        LocalData[ LocalPosY ] = Temp.lo;
        LocalMask[ LocalPosY ] = Temp.s4;
    }
    else
    {
        LocalData[ LocalPosY ] = (int4)(0);
        LocalMask[ LocalPosY ] = 0;
    }

    for( ; Row <= GlobalPosY + Radius; Row += LocalSizeY )
    {
        // Read the second part into local memory
        if( IsInside( Row + LocalSizeY, GlobalPosX, Height, Width ) )
        {
            Temp = ImIn[ Index( Row + LocalSizeY, GlobalPosX, Height, Width ) ];
			LocalData[LocalPosY + LocalSizeY] = Temp.lo;
			LocalMask[LocalPosY + LocalSizeY] = Temp.s4;
        }
        else
        {
			LocalData[LocalPosY + LocalSizeY] = (int4)(0);
            LocalMask[ LocalPosY + LocalSizeY ] = 0;
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        for( int i = 0; i < LocalSizeY && (Row + i <= GlobalPosY + Radius); ++i )
        {
            Sum = Sum + LocalData[ LocalPosY + i ];
            Counter = Counter + (int4)(LocalMask[ LocalPosY + i ]);
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        // Copy the second part into the first part
        LocalData[ LocalPosY ] = LocalData[ LocalPosY + LocalSizeY ];
        LocalMask[ LocalPosY ] = LocalMask[ LocalPosY + LocalSizeY ];
    }
    
    if( Counter.x > 0 )
    {
        DSum = convert_double4( Sum );
        DSum = DSum / convert_double4( Counter );
    }
    
	if (IsInsideMask(GlobalPosY, GlobalPosX, Height, Width, Mask))
	{
		uchar4 out_val = convert_uchar4(DSum);
		//uchar4 out_val = convert_uchar4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)].lo);
		out_val.w = 255;
		ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = out_val;
	}
    else if( IsInside( GlobalPosY, GlobalPosX, Height, Width ) )
        ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = (uchar4)(0);
}

__kernel void MeanFilter_WithMask_H_NoLocal(__global const uchar4 *ImIn, __global const uchar *Mask,
	__global int8 *ImOut, int Height, int Width, int Radius)
{
	const int GlobalPosY = get_global_id(1);
	const int GlobalPosX = get_global_id(0);

	int4 Sum = 0;
	int8 DSum = (int8)(0);
	int4 Counter = 0;

	int Col = GlobalPosX - Radius;

	for (; Col <= GlobalPosX + Radius; Col += 1)
	{
		// Read the second part
		if (IsInsideMask(GlobalPosY, Col, Height, Width, Mask))
		{
			Sum += convert_int4(ImIn[Index(GlobalPosY, Col, Height, Width)]);
			Counter += (int4)(1);
		}
	}

	if (Counter.s0 > 0)
	{
		DSum.lo = convert_int4(Sum);
		//DSum.lo = convert_int4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)]);
		DSum.hi = convert_int4(Counter);
	}

	if (IsInside(GlobalPosY, GlobalPosX, Height, Width))
		ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = DSum;
}

__kernel void MeanFilter_WithMask_V_NoLocal(__global int8 *ImIn, __global const uchar *Mask,
	__global uchar4 *ImOut, int Height, int Width, int Radius)
{

	const int GlobalPosY = get_global_id(0);
	const int GlobalPosX = get_global_id(1);

	int4 Sum = 0;
	double4 DSum = 0.0;
	int4 Counter = 0;
	int OffsetY = 0;
	int8 Temp;
	int Row = GlobalPosY - Radius;

	for (; Row <= GlobalPosY + Radius; Row += 1)
	{
		// Read the second part into local memory
		if (IsInside(Row, GlobalPosX, Height, Width))
		{
			Temp = ImIn[Index(Row, GlobalPosX, Height, Width)];
			Sum = Sum + Temp.lo;
			Counter = Counter + Temp.s4;
		}
	}

	if (Counter.x > 0)
	{
		DSum = convert_double4(Sum);
		DSum = DSum / convert_double4(Counter);
	}

	if (IsInsideMask(GlobalPosY, GlobalPosX, Height, Width, Mask))
	{
		uchar4 out_val = convert_uchar4(DSum);
		//uchar4 out_val = convert_uchar4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)].lo);
		out_val.w = 255;
		ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = out_val;
	}
	else if (IsInside(GlobalPosY, GlobalPosX, Height, Width))
		ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = (uchar4)(0);
}

__kernel void MeanFilter_WithMask_V_Transpose( __global int8 *ImIn, __global const uchar *Mask,
                                               __global uchar4 *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int4 LocalData[ 64 ];
    __local int LocalMask[ 64 ];
    
    const int GlobalPosY = get_global_id( 0 );
    const int GlobalPosX = get_global_id( 1 );
    
    const int LocalPosY = get_local_id( 0 );
    const int LocalPosX = get_local_id( 1 );
    
    const int LocalSizeY = get_local_size( 0 );
    const int LocalSizeX = get_local_size( 1 );
    
    int4 Sum = 0;
    double4 DSum = 0.0;
    int4 Counter = 0;
    int OffsetY = 0;
    int8 Temp;
    int Row = GlobalPosY - Radius;
    
    // First read into local memory (tranposed matrix)
    if( IsInside( Row, GlobalPosX, Height, Width ) )
    {
        Temp = ImIn[ IndexTranspose( Row, GlobalPosX, Height, Width ) ];
        LocalData[ LocalPosY ] = Temp.lo;
        LocalMask[ LocalPosY ] = Temp.s4;
    }
    else
    {
        LocalData[ LocalPosY ] = (int4)(0);
        LocalMask[ LocalPosY ] = 0;
    }

    for( ; Row <= GlobalPosY + Radius; Row += LocalSizeY )
    {
        // Read the second part into local memory
        if( IsInside( Row + LocalSizeY, GlobalPosX, Height, Width ) )
        {
            Temp = ImIn[ IndexTranspose( Row + LocalSizeY, GlobalPosX, Height, Width ) ];
            LocalData[ LocalPosY + LocalSizeY ] = Temp.lo;
            LocalMask[ LocalPosY + LocalSizeY ] = Temp.s4;
        }
        else
        {
			LocalData[LocalPosY + LocalSizeY] = (int4)(0);
            LocalMask[ LocalPosY + LocalSizeY ] = 0;
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        for( int i = 0; i < LocalSizeY && (Row + i <= GlobalPosY + Radius); ++i )
        {
            Sum = Sum + LocalData[ LocalPosY + i ];
            Counter = Counter + (int4)(LocalMask[ LocalPosY + i ]);
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        // Copy the second part into the first part
        LocalData[ LocalPosY ] = LocalData[ LocalPosY + LocalSizeY ];
        LocalMask[ LocalPosY ] = LocalMask[ LocalPosY + LocalSizeY ];
    }
    
    if( Counter.x > 0 )
    {
		DSum = convert_double4(Sum);
		DSum = DSum / convert_double4(Counter);
    }
    
	if (IsInsideMask(GlobalPosY, GlobalPosX, Height, Width, Mask))
	{
		uchar4 out_val = convert_uchar4(DSum);
		//uchar4 out_val = convert_uchar4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)].lo);
		out_val.w = 255;
		ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = out_val;
	}
    else if( IsInside( GlobalPosY, GlobalPosX, Height, Width ) )
        ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = (uchar4)(0);
}

__kernel void TempTranspose( __global int8 *ImIn, __global int8 *ImOut, int Height, int Width )
{
    __local int8 LocalData[ 16 ][ 16 ];
    
    const int GlobalPosY = get_global_id( 1 );
    const int GlobalPosX = get_global_id( 0 );
    
    const int LocalPosY = get_local_id( 1 );
    const int LocalPosX = get_local_id( 0 );
    
    const int LocalSizeX = get_local_size( 0 );
    const int LocalSizeY = get_local_size( 1 );
    
    const int GroupPosX = get_group_id( 0 );
    const int GroupPosY = get_group_id( 1 );
    
    const int TransposedPosX = GroupPosY * LocalSizeY + LocalPosX;
    const int TransposedPosY = GroupPosX * LocalSizeX + LocalPosY;
    
    if( IsInside( GlobalPosY, GlobalPosX, Height, Width ) )
    {
        LocalData[ LocalPosY ][ LocalPosX ] = ImIn[ Index( GlobalPosY, GlobalPosX, Height, Width ) ];
    }
        
    barrier( CLK_LOCAL_MEM_FENCE );
        
    if( IsInside( TransposedPosY, TransposedPosX, Width, Height ) )
    {
        // Invert position in local data, and invert height/width in global data
        ImOut[ Index( TransposedPosY, TransposedPosX, Width, Height ) ] = LocalData[ LocalPosX ][ LocalPosY ];
    }
}

__kernel void TempTranspose2( __global int2 *ImIn, __global int2 *ImOut, int Height, int Width )
{
    __local int2 LocalData[ 16 ][ 16 ];

    const int GlobalPosY = get_global_id( 1 );
    const int GlobalPosX = get_global_id( 0 );

    const int LocalPosY = get_local_id( 1 );
    const int LocalPosX = get_local_id( 0 );

    const int LocalSizeX = get_local_size( 0 );
    const int LocalSizeY = get_local_size( 1 );

    const int GroupPosX = get_group_id( 0 );
    const int GroupPosY = get_group_id( 1 );

    const int TransposedPosX = GroupPosY * LocalSizeY + LocalPosX;
    const int TransposedPosY = GroupPosX * LocalSizeX + LocalPosY;

    if( IsInside( GlobalPosY, GlobalPosX, Height, Width ) )
    {
        LocalData[ LocalPosY ][ LocalPosX ] = ImIn[ Index( GlobalPosY, GlobalPosX, Height, Width ) ];
    }

    barrier( CLK_LOCAL_MEM_FENCE );

    if( IsInside( TransposedPosY, TransposedPosX, Width, Height ) )
    {
        // Invert position in local data, and invert height/width in global data
        ImOut[ Index( TransposedPosY, TransposedPosX, Width, Height ) ] = LocalData[ LocalPosX ][ LocalPosY ];
    }
}

__kernel void MeanFilterPad_H( __global const uchar4 *ImIn, __global const uchar *ImMask, __global int8 *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int4 LocalData[ MEMSIZE ];
    __local int LocalMask[ MEMSIZE ];
    
    const int GlobalPosY = get_global_id( 1 ) + Radius;
    const int GlobalPosX = get_global_id( 0 ) + Radius;
    
    const int LocalPosX = get_local_id( 0 );
    
    const int LocalSizeX = get_local_size( 0 );
    
    int4 Sum = 0;
    int8 DSum = (int8)(0);
    int4 Counter = 0;
    
    int Col = GlobalPosX - Radius;
    
    // First read
    LocalData[ LocalPosX ] = convert_int4( ImIn[ Index( GlobalPosY, Col, Height, Width ) ] );
    LocalMask[ LocalPosX ] = convert_int( ImMask[ Index( GlobalPosY, Col, Height, Width ) ] );

    for( ; Col <= GlobalPosX + Radius; Col += LocalSizeX )
    {

        // Read the second part
        LocalData[ LocalPosX + LocalSizeX ] = convert_int4( ImIn[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ] );
        LocalMask[ LocalPosX + LocalSizeX ] = convert_int( ImMask[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ] );
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        for( int j = 0; j < LocalSizeX && (Col + j <= GlobalPosX + Radius); ++j )
        {
            Sum = LocalData[ LocalPosX + j ] + Sum;
            Counter = Counter + (int4)(LocalMask[ LocalPosX + j ]);
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        // Copy the second part into the first part of local memory
        LocalData[ LocalPosX ] = LocalData[ LocalPosX + LocalSizeX ];
        LocalMask[ LocalPosX ] = LocalMask[ LocalPosX + LocalSizeX ];
    }
    
	DSum.lo = Sum;
	//DSum.lo = convert_int4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)]);
	DSum.hi = Counter;
        
    ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = DSum;
}

__kernel void MeanFilterPad2_H( __global const uchar *ImIn, __global const uchar *ImMask, __global int2 *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int LocalData[ MEMSIZE ];
    __local int LocalMask[ MEMSIZE ];

    const int GlobalPosY = get_global_id( 1 ) + Radius;
    const int GlobalPosX = get_global_id( 0 ) + Radius;

    const int LocalPosX = get_local_id( 0 );

    const int LocalSizeX = get_local_size( 0 );

    int Sum = 0;
    int2 DSum = (int2)(0,0);
    int Counter = 0;

    int Col = GlobalPosX - Radius;

    // First read
    LocalData[ LocalPosX ] = convert_int( ImIn[ Index( GlobalPosY, Col, Height, Width ) ] );
    LocalMask[ LocalPosX ] = convert_int( ImMask[ Index( GlobalPosY, Col, Height, Width ) ] );

    for( ; Col <= GlobalPosX + Radius; Col += LocalSizeX )
    {

        // Read the second part
        LocalData[ LocalPosX + LocalSizeX ] = convert_int( ImIn[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ] );
        LocalMask[ LocalPosX + LocalSizeX ] = convert_int( ImMask[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ] );

        barrier( CLK_LOCAL_MEM_FENCE );

        for( int j = 0; j < LocalSizeX && (Col + j <= GlobalPosX + Radius); ++j )
        {
            Sum = LocalData[ LocalPosX + j ] + Sum;
            Counter = Counter + LocalMask[ LocalPosX + j ];
        }

        barrier( CLK_LOCAL_MEM_FENCE );

        // Copy the second part into the first part of local memory
        LocalData[ LocalPosX ] = LocalData[ LocalPosX + LocalSizeX ];
        LocalMask[ LocalPosX ] = LocalMask[ LocalPosX + LocalSizeX ];
    }

    DSum = (int2)( Sum, Counter );

    ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = DSum;
}

__kernel void MeanFilterPad_V( __global int8 *ImIn, __global uchar4 *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int4 LocalData[ MEMSIZE ];
    __local int LocalMask[ MEMSIZE ];
    
    const int GlobalPosY = get_global_id( 0 ) + Radius;
    const int GlobalPosX = get_global_id( 1 ) + Radius;
    
    const int LocalPosY = get_local_id( 0 );
    
    const int LocalSizeY = get_local_size( 0 );
    
    int4 Sum = 0;
    double4 DSum = 0.0;
    int4 Counter = 0;
    int8 Temp;
    int Row = GlobalPosY - Radius;
    
    // First read into local memory
    Temp = ImIn[ Index( Row, GlobalPosX, Height, Width ) ];
    LocalData[ LocalPosY ] = Temp.lo;
    LocalMask[ LocalPosY ] = Temp.s4;

    for( ; Row <= GlobalPosY + Radius; Row += LocalSizeY )
    {
        // Read the second part into local memory
        Temp = ImIn[ Index( Row + LocalSizeY, GlobalPosX, Height, Width ) ];
        LocalData[ LocalPosY + LocalSizeY ] = Temp.lo;
        LocalMask[ LocalPosY + LocalSizeY ] = Temp.s4;
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        for( int i = 0; i < LocalSizeY && (Row + i <= GlobalPosY + Radius); ++i )
        {
            Sum = Sum + LocalData[ LocalPosY + i ];
            Counter = Counter + (int4)(LocalMask[ LocalPosY + i ]);
        }
        
        barrier( CLK_LOCAL_MEM_FENCE );
        
        // Copy the second part into the first part
        LocalData[ LocalPosY ] = LocalData[ LocalPosY + LocalSizeY ];
        LocalMask[ LocalPosY ] = LocalMask[ LocalPosY + LocalSizeY ];
    }
    
	if (Counter.x > 0)
	{
		DSum = convert_double4(Sum);
		DSum = DSum / convert_double4(Counter);
	}

	uchar4 out_val = convert_uchar4(DSum);
	//uchar4 out_val = convert_uchar4(ImIn[Index(GlobalPosY, GlobalPosX, Height, Width)].lo);
	out_val.w = 255;
	ImOut[Index(GlobalPosY, GlobalPosX, Height, Width)] = out_val;
}

__kernel void MeanFilterPad_V_Transpose( __global int2 *ImIn, __global uchar *ImOut, int Height, int Width, int Radius )
{
    // 2 * localsize
    __local int LocalData[ MEMSIZE ];
    __local int LocalMask[ MEMSIZE ];

    const int GlobalPosY = get_global_id( 0 ) + Radius;
    const int GlobalPosX = get_global_id( 1 ) + Radius;

    const int LocalPosY = get_local_id( 0 );

    const int LocalSizeY = get_local_size( 0 );

    int Sum = 0;
    double DSum = 0.0;
    int Counter = 0;
    int2 Temp;
    int Row = GlobalPosY - Radius;

    // First read into local memory
    Temp = ImIn[ IndexTranspose( Row, GlobalPosX, Height, Width ) ];
    LocalData[ LocalPosY ] = Temp.x;
    LocalMask[ LocalPosY ] = Temp.y;

    for( ; Row <= GlobalPosY + Radius; Row += LocalSizeY )
    {
        // Read the second part into local memory
        Temp = ImIn[ IndexTranspose( Row + LocalSizeY, GlobalPosX, Height, Width ) ];
        LocalData[ LocalPosY + LocalSizeY ] = Temp.x;
        LocalMask[ LocalPosY + LocalSizeY ] = Temp.y;

        barrier( CLK_LOCAL_MEM_FENCE );

        for( int i = 0; i < LocalSizeY && (Row + i <= GlobalPosY + Radius); ++i )
        {
            Sum = Sum + LocalData[ LocalPosY + i ];
            Counter = Counter + LocalMask[ LocalPosY + i ];
        }

        barrier( CLK_LOCAL_MEM_FENCE );

        // Copy the second part into the first part
        LocalData[ LocalPosY ] = LocalData[ LocalPosY + LocalSizeY ];
        LocalMask[ LocalPosY ] = LocalMask[ LocalPosY + LocalSizeY ];
    }

    if( Counter > 0 )
    {
        DSum = (double)( Sum );
        DSum = DSum / (double)( Counter );
    }

    ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = convert_uchar( DSum );
}

__kernel void MeanFilterPadAligned_H( __global const uchar *ImIn, __global const uchar *ImMask, __global int2 *ImOut, int Height, int Width, int Radius, int ByteAlign )
{
    // 2 * localsize
    __local int LocalData[ MEMSIZE ];
    __local int LocalMask[ MEMSIZE ];

    const int GlobalPosY = get_global_id( 1 ) + Radius + ByteAlign;
    const int GlobalPosX = get_global_id( 0 ) + Radius + ByteAlign;

    const int LocalPosX = get_local_id( 0 );

    const int LocalSizeX = get_local_size( 0 );

    int Sum = 0;
    int2 DSum = (int2)(0,0);
    int Counter = 0;

    int Col = GlobalPosX - Radius;

    // First read
    LocalData[ LocalPosX ] = convert_int( ImIn[ Index( GlobalPosY, Col, Height, Width ) ] );
    LocalMask[ LocalPosX ] = convert_int( ImMask[ Index( GlobalPosY, Col, Height, Width ) ] );

    for( ; Col <= GlobalPosX + Radius; Col += LocalSizeX )
    {

        // Read the second part
        LocalData[ LocalPosX + LocalSizeX ] = convert_int( ImIn[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ] );
        LocalMask[ LocalPosX + LocalSizeX ] = convert_int( ImMask[ Index( GlobalPosY, Col + LocalSizeX, Height, Width ) ] );

        barrier( CLK_LOCAL_MEM_FENCE );

        for( int j = 0; j < LocalSizeX && (Col + j <= GlobalPosX + Radius); ++j )
        {
            Sum = LocalData[ LocalPosX + j ] + Sum;
            Counter = Counter + LocalMask[ LocalPosX + j ];
        }

        barrier( CLK_LOCAL_MEM_FENCE );

        // Copy the second part into the first part of local memory
        LocalData[ LocalPosX ] = LocalData[ LocalPosX + LocalSizeX ];
        LocalMask[ LocalPosX ] = LocalMask[ LocalPosX + LocalSizeX ];
    }

    DSum = (int2)( Sum, Counter );

    ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = DSum;
}

__kernel void MeanFilterPadAligned_V_Transpose( __global int2 *ImIn, __global uchar *ImOut, int Height, int Width, int Radius, int ByteAlign )
{
    // 2 * localsize
    __local int LocalData[ MEMSIZE ];
    __local int LocalMask[ MEMSIZE ];

    const int GlobalPosY = get_global_id( 0 ) + Radius + ByteAlign;
    const int GlobalPosX = get_global_id( 1 ) + Radius + ByteAlign;

    const int LocalPosY = get_local_id( 0 );

    const int LocalSizeY = get_local_size( 0 );

    int Sum = 0;
    double DSum = 0.0;
    int Counter = 0;
    int2 Temp;
    int Row = GlobalPosY - Radius;

    // First read into local memory
    Temp = ImIn[ IndexTranspose( Row, GlobalPosX, Height, Width ) ];
    LocalData[ LocalPosY ] = Temp.x;
    LocalMask[ LocalPosY ] = Temp.y;

    for( ; Row <= GlobalPosY + Radius; Row += LocalSizeY )
    {
        // Read the second part into local memory
        Temp = ImIn[ IndexTranspose( Row + LocalSizeY, GlobalPosX, Height, Width ) ];
        LocalData[ LocalPosY + LocalSizeY ] = Temp.x;
        LocalMask[ LocalPosY + LocalSizeY ] = Temp.y;

        barrier( CLK_LOCAL_MEM_FENCE );

        for( int i = 0; i < LocalSizeY && (Row + i <= GlobalPosY + Radius); ++i )
        {
            Sum = Sum + LocalData[ LocalPosY + i ];
            Counter = Counter + LocalMask[ LocalPosY + i ];
        }

        barrier( CLK_LOCAL_MEM_FENCE );

        // Copy the second part into the first part
        LocalData[ LocalPosY ] = LocalData[ LocalPosY + LocalSizeY ];
        LocalMask[ LocalPosY ] = LocalMask[ LocalPosY + LocalSizeY ];
    }

    if( Counter > 0 )
    {
        DSum = (double)( Sum );
        DSum = DSum / (double)( Counter );
    }

    ImOut[ Index( GlobalPosY, GlobalPosX, Height, Width ) ] = convert_uchar( DSum );
}

__kernel void ZeroOut( __global int8 *Data )
{
    const int GlobalPos = get_global_id( 0 );
    int8 Zero = (int8)( 0 );
    Data[ GlobalPos ] = Zero;
}
