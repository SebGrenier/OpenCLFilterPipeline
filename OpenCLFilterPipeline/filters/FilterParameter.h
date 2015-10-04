#ifndef __FILTERS_FILTERPARAMETER_H__
#define __FILTERS_FILTERPARAMETER_H__

#include <QVariant>
#include <QString>
#include <QMap>
#include <QList>
#include <QPair>
#include <exception>

class AFilterParameter
{
public:
    // Types definition's enum
    enum FilterParameter_Type{ FP_INT
                               , FP_FLOAT
                               , FP_DOUBLE
                               , FP_STRING
                               , FP_CHAR
                               , FP_BOOL
                             };


    AFilterParameter( const QVariant &Val,
                      const QString &Name,
                      const QString &Description,
                      const QVariant &Min,
                      const QVariant &Max,
                      const FilterParameter_Type Type );
    AFilterParameter( const AFilterParameter &Param );
    virtual ~AFilterParameter( void ) { }

    inline const QString&             GetName        ( void ) const { return m_Name;         }
    inline const QString&             GetDescription ( void ) const { return m_Description;  }
    inline const FilterParameter_Type GetType        ( void ) const { return m_Type;         }
    inline const QVariant&            GetValue       ( void ) const { return m_Value;        }
    inline       QVariant&            GetValue       ( void )       { return m_Value;        }
    inline const QVariant&            GetMin         ( void ) const { return m_MinValue;     }
    inline       QVariant&            GetMin         ( void )       { return m_MinValue;     }
    inline const QVariant&            GetMax         ( void ) const { return m_MaxValue;     }
    inline       QVariant&            GetMax         ( void )       { return m_MaxValue;     }
    inline const QVariant&            GetDefaultValue( void ) const { return m_DefaultValue; }

    inline void SetValue( const QVariant &Val, const FilterParameter_Type Type )
    {
        m_Value = Val;
        m_Type = Type;
        this->ClampValue( );
    }

    virtual AFilterParameter* Clone( void ) = 0;

protected:
    virtual void ClampValue( void ) = 0;


    QString m_Name;
    QString m_Description;

    QVariant m_Value;
    QVariant m_MinValue;
    QVariant m_MaxValue;
    QVariant m_DefaultValue;

    FilterParameter_Type m_Type;
};

template < typename T >
class CFilterParameterInterpreter
{
public:
    CFilterParameterInterpreter( void ) { }
    virtual ~CFilterParameterInterpreter( void ) { }

    static T Convert( const AFilterParameter* const Param )
    {
        QVariant Val = Param->GetValue( );
        return Convert( Val );
    }

    static T Convert( const QVariant &Val )
    {
        if( Val.canConvert< T >( ) )
        {
            T TempVal = Val.value<T>( );
            return TempVal;
        }
        else
        {
            throw std::exception( "Cannot convert parameter." );
        }
    }

    static void Assign( AFilterParameter* Param, const T &Val )
    {
        Param->SetValue( QVariant( Val ), Param->GetType( ) );
    }

protected:
};


class CFilterInt : public AFilterParameter
{
public:
    CFilterInt( const int Val,
                const QString &Name,
                const QString &Description,
                const int Min,
                const int Max )
        : AFilterParameter( QVariant( Val ),
                            Name,
                            Description,
                            QVariant( Min ),
                            QVariant( Max ),
                            AFilterParameter::FP_INT )
    { }
    virtual ~CFilterInt( void ) { }

    virtual AFilterParameter* Clone( void );

    inline int GetValueInt( void ) const
    {
        return CFilterParameterInterpreter<int>::Convert( m_Value );
    }

protected:
    virtual void ClampValue( void )
    {
        int Min = m_MinValue.value<int>( );
        int Max = m_MaxValue.value<int>( );
        int Val = m_Value.value<int>( );

        Val = std::min( Max, std::max(Min, Val));

        m_Value.setValue( Val );
    }

private:
};

class CFilterFloat : public AFilterParameter
{
public:
    CFilterFloat( const float Val,
                  const QString &Name,
                  const QString &Description,
                  const float Min,
                  const float Max,
                  const int Decimal = 2,
                  const float Step = 0.01f )
        : AFilterParameter( QVariant( Val ),
                            Name,
                            Description,
                            QVariant( Min ),
                            QVariant( Max ),
                            AFilterParameter::FP_FLOAT )
        , m_Decimal( Decimal )
        , m_Step( Step )
    { }
    virtual ~CFilterFloat( void ) { }

    virtual AFilterParameter* Clone( );

    inline float GetValueFloat( void ) const
    {
        return CFilterParameterInterpreter<float>::Convert( m_Value );
    }

    inline int    GetDecimal( void ) const { return m_Decimal; }
    inline double GetStep   ( void ) const { return m_Step;    }

protected:
    virtual void ClampValue( void )
    {
        float Min = m_MinValue.value<float>( );
        float Max = m_MaxValue.value<float>( );
        float Val = m_Value.value<float>( );

		Val = std::min(Max, std::max(Min, Val));

        m_Value.setValue( Val );
    }

    int   m_Decimal;
    float m_Step;
};

class CFilterDouble : public AFilterParameter
{
public:
    CFilterDouble( const double Val,
                   const QString &Name,
                   const QString &Description,
                   const double Min,
                   const double Max,
                   const int Decimal = 2,
                   const double Step = 0.01 )
        : AFilterParameter( QVariant( Val ),
                            Name,
                            Description,
                            QVariant( Min ),
                            QVariant( Max ),
                            AFilterParameter::FP_DOUBLE )
        , m_Decimal( Decimal )
        , m_Step( Step )
    { }
    virtual ~CFilterDouble( void ){ }

    virtual AFilterParameter* Clone( );

    inline double GetValueDouble( void ) const
    {
        return CFilterParameterInterpreter<double>::Convert( m_Value );
    }

    inline int    GetDecimal( void ) const { return m_Decimal; }
    inline double GetStep   ( void ) const { return m_Step;    }

protected:
    virtual void ClampValue( void )
    {
        double Min = m_MinValue.value<double>( );
        double Max = m_MaxValue.value<double>( );
        double Val = m_Value.value<double>( );

		Val = std::min(Max, std::max(Min, Val));

        m_Value.setValue( Val );
    }

    int    m_Decimal;
    double m_Step;
};

class CFilterString : public AFilterParameter
{
public:
    CFilterString( const QString &Val,
                   const QString &Name,
                   const QString &Description,
                   const QString &Min,
                   const QString &Max )
        : AFilterParameter( QVariant( Val ),
                            Name,
                            Description,
                            QVariant( Min ),
                            QVariant( Max ),
                            AFilterParameter::FP_STRING )
    { }
    virtual ~CFilterString( void ) { }

    virtual AFilterParameter* Clone( );

    inline QString GetValueString( void ) const
    {
        return CFilterParameterInterpreter<QString>::Convert( m_Value );
    }

protected:
    virtual void ClampValue( void )
    {
        QString Min = m_MinValue.value<QString>( );
        QString Max = m_MaxValue.value<QString>( );
        QString Val = m_Value.value<QString>( );

        if( Val < Min )
            Val = Min;
        if( Val > Max )
            Val = Max;

        m_Value.setValue( Val );
    }
};

class CFilterChar : public AFilterParameter
{
public:
    CFilterChar( const char Val,
                 const QString &Name,
                 const QString &Description,
                 const char Min,
                 const char Max )
        : AFilterParameter( QVariant( Val ),
                            Name,
                            Description,
                            QVariant( Min ),
                            QVariant( Max ),
                            AFilterParameter::FP_CHAR )
    { }
    virtual ~CFilterChar( void ) { }

    virtual AFilterParameter* Clone( );

    inline char GetValueChar( void ) const
    {
        return CFilterParameterInterpreter<char>::Convert( m_Value );
    }

protected:
    virtual void ClampValue( void )
    {
        char Min = m_MinValue.value<char>( );
        char Max = m_MaxValue.value<char>( );
        char Val = m_Value.value<char>( );

		Val = std::min(Max, std::max(Min, Val));

        m_Value.setValue( Val );
    }
};

class CFilterBool : public AFilterParameter
{
public:
    CFilterBool( const bool Val,
                 const QString &Name,
                 const QString &Description,
                 const bool Min = false,
                 const bool Max = true
               )
        : AFilterParameter( QVariant( Val ),
                            Name,
                            Description,
                            QVariant( Min ),
                            QVariant( Max ),
                            AFilterParameter::FP_BOOL )
    { }
    virtual ~CFilterBool( void ){ }

    virtual AFilterParameter* Clone( );

    inline bool GetValueBool( void ) const
    {
        return CFilterParameterInterpreter<bool>::Convert( m_Value );
    }

protected:
    virtual void ClampValue( void )
    { }
};

/**
  @class CFilterParameterMap
  @brief This class holds a list of AFilterParameter*. When you assign
         a new AFilterParameter to the list, it becomes the owner of the
         parameter.
  @author Sebastien Grenier
  @date 2012-10-24
  @note It is a list, not a map.
  **/
class CFilterParameterMap
{
public:
    typedef QList< AFilterParameter* > Map_Type;
    typedef QList< QPair< QString, QString > > Toggle_List;
    typedef Map_Type::iterator Iterator;
    typedef Map_Type::const_iterator CstIterator;
    typedef Toggle_List::iterator ToggleIterator;
    typedef Toggle_List::const_iterator ToggleCstIterator;

    CFilterParameterMap( void );
    CFilterParameterMap( const CFilterParameterMap &Params );
    ~CFilterParameterMap( void );

    void AddParameter( AFilterParameter *Param );
    void RemoveParameter( const QString &Name );
    void AddToggledParameter( const QString &ParamName, const QString &TogglerName );
    bool IsToggledParameter( const QString &ParamName ) const;

    const AFilterParameter* const GetParameter( const QString &Name ) const;
    AFilterParameter*             GetParameter( const QString &Name );

    const QString& GetTogglingParameter( const QString &ToggledParamName ) const;

    inline Iterator    Begin( void )       { return m_ParamList.begin( ); }
    inline CstIterator Begin( void ) const { return m_ParamList.begin( ); }

    inline Iterator    End( void )       { return m_ParamList.end( ); }
    inline CstIterator End( void ) const { return m_ParamList.end( ); }

    CFilterParameterMap& operator=( const CFilterParameterMap &Params );

    bool Contains( const QString &Name ) const;

    Iterator    Find( const QString &Name );
    CstIterator Find( const QString &Name ) const;

private:
    Map_Type m_ParamList;

    Toggle_List m_ToggledParams;
};

#endif // __FILTERS_FILTERPARAMETER_H__
