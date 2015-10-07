#ifndef __MATHUTILS_H__
#define __MATHUTILS_H__

#include <algorithm>
namespace Utils
{
    /**
        * @brief Get the next multiple of a number X after or directly
        * on the number N.
        * @param Value : Starting value.
        * @param Basis : Basis of the operation.
        */
    template< typename T >
    T GetNextMultipleOf( const T Value, const T Basis )
    {
        if( Basis == T(0) )
            return Value;

        T Temp = ( Value + Basis - 1 ) / Basis;
        Temp *= Basis;
        return Temp;
    }

    template <> float GetNextMultipleOf( const float Value, const float Basis );
    template <> double GetNextMultipleOf( const double Value, const double Basis );

    /**
        * @brief Test if two values are equal, relative to an epsilon.
        * @param Val : The first value.
        * @param Ref : The second value.
        * @param Epsilon : The epsilon value. Defaults to 0.
        */
    template< typename T> bool IsEqual( const T Val, const T Ref, const T Epsilon = T(0) )
    {
        return abs( Val - Ref ) <= Epsilon;
    }

    template<> bool IsEqual( const float Val, const float Ref, const float Epsilon );

    template<> bool IsEqual( const double Val, const double Ref, const double Epsilon );

    /**
        * @brief Abstract class defining a rounding operator
        */
    template<typename T>
    class RoundMode
    {
    public:
        RoundMode( void ) {}
        virtual ~RoundMode( void ) {}


        virtual T operator () ( const T Val, const int NbDigits ) const = 0;
    };

    /**
        * @brief Implementation of a round down operator.
        * Truncates a floating point number up to a certain number of
        * digits.
        */
    template<typename T>
    class RoundDown : public RoundMode<T>
    {
    public:
        RoundDown( void ) {}
        virtual ~RoundDown( void ) {}

        virtual T operator () ( const T Val, const int NbDigits ) const;
    };

    template<>
    float RoundDown<float>::operator()( const float Val, const int NbDigits ) const;

    template<>
    double RoundDown<double>::operator()( const double Val, const int NbDigits ) const;

    /**
        * @brief Implementation of a round to nearest operator.
        * Rounds a floating point number to the nearest value
        * up to a certain number of digits.
        */
    template<typename T>
    class RoundNearest : public RoundMode<T>
    {
    public:
        RoundNearest( void ) {}
        virtual ~RoundNearest( void ) {}

        virtual T operator () ( const T Val, const int NbDigits ) const;
    };

    template<>
    float RoundNearest<float>::operator()( const float Val, const int NbDigits ) const;

    template<>
    double RoundNearest<double>::operator()( const double Val, const int NbDigits ) const;

    /*! @fn template<typename T> T Round(const T Val, const int NbDigits, const RoundMode<T> Mode=RoundNearest<T>())
        * @brief Round a floating point value to NbDigits using the specified rounding mode. (Default is round nearest)
        * @param Val : The value to round
        * @param NbDigits : The number of digits to conserve
        * @param Mode : The rounding Mode
        */
    template< typename T >
    T Round( const T Val, const int NbDigits, const RoundMode<T>& Mode=RoundNearest<T>() )
    {
        return Mode( Val, NbDigits );
    }

    /*! @fn template<typename T> T Clamp(const T Val, const T Min, const T Max)
        * @brief Clamp a value in the specified interval
        * @param Val : The value to clamp
        * @param Min : The minimum value
        * @param Max : The maximum value
        */
    template< typename T >
    T Clamp( const T Val, const T Min, const T Max )
    {
        return std::min( Max, std::max( Min, Val ) );
    }
}

#endif // __MATHUTILS_H__
