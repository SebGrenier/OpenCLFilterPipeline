#include "MathUtils.h"

#include <cmath>

namespace Utils
{
    template < >
    float GetNextMultipleOf( const float Value, const float Basis )
    {
        if( Basis == 0.0f )
            return Value;

        float Temp = floor( ( Value + Basis - 1 ) / Basis );
        Temp *= Basis;
        return Temp;
    }

    template < >
    double GetNextMultipleOf( const double Value, const double Basis )
    {
        if( Basis == 0.0 )
            return Value;

        double Temp = floor( ( Value + Basis - 1 ) / Basis );
        Temp *= Basis;
        return Temp;
    }

    template<>
    bool IsEqual<float>( const float Val, const float Ref, const float Epsilon )
    {
        return fabsf( Val - Ref ) <= Epsilon;
    }

    template<>
    bool IsEqual<double>( const double Val, const double Ref, const double Epsilon )
    {
        return fabs( Val - Ref ) <= Epsilon;
    }

    template<>
    float RoundDown<float>::operator()( const float Val, const int NbDigits ) const
    {
        const float Factor = (float)pow( 10, NbDigits );
        const int Temp = int( floorf( Val * Factor ) );
        return float( Temp ) / Factor;
    }

    template<>
    double RoundDown<double>::operator()( const double Val, const int NbDigits ) const
    {
        const double Factor = (double)pow( 10, NbDigits );
        const long long Temp = (long long)( floor( Val * Factor ) );
        return double( Temp ) / Factor;
    }

    template<>
    float RoundNearest<float>::operator()( const float Val, const int NbDigits ) const
    {
        const float Factor = (float)pow( 10, NbDigits );
        const int Temp = int( floorf( Val * Factor + 0.5f ) );
        return float( Temp ) / Factor;
    }

    template<>
    double RoundNearest<double>::operator()( const double Val, const int NbDigits ) const
    {
        const double Factor = (double)pow( 10, NbDigits );
        const long long Temp = long long( floor( Val * Factor + 0.5 ) );
        return double( Temp ) / Factor;
    }
}
