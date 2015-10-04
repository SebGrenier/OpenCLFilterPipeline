#include "FilterParameter.h"

AFilterParameter::AFilterParameter( const QVariant &Val,
                                    const QString &Name,
                                    const QString &Description,
                                    const QVariant &Min,
                                    const QVariant &Max,
                                    const FilterParameter_Type Type )
    : m_Name        ( Name        )
    , m_Description ( Description )
    , m_Value       ( Val         )
    , m_MinValue    ( Min         )
    , m_MaxValue    ( Max         )
    , m_DefaultValue( Val         )
    , m_Type        ( Type        )
{ }

AFilterParameter::AFilterParameter( const AFilterParameter &Param )
    : m_Name        ( Param.m_Name         )
    , m_Description ( Param.m_Description  )
    , m_Value       ( Param.m_Value        )
    , m_MinValue    ( Param.m_MinValue     )
    , m_MaxValue    ( Param.m_MaxValue     )
    , m_DefaultValue( Param.m_DefaultValue )
    , m_Type        ( Param.m_Type         )
{ }

// Use a define for better code reuse
#define CLONE_DEF( CLASS, TYPE )                       \
    AFilterParameter* CLASS::Clone( void )             \
    {                                                  \
        return new CLASS( m_Value.value<TYPE>( ),      \
                          m_Name,                      \
                          m_Description,               \
                          m_MinValue.value<TYPE>( ),   \
                          m_MaxValue.value<TYPE>( ) ); \
    }                                                  \

#define CLONE_NUMERIC_DEF( CLASS, TYPE )               \
    AFilterParameter* CLASS::Clone( void )             \
    {                                                  \
        return new CLASS( m_Value.value<TYPE>( ),      \
                          m_Name,                      \
                          m_Description,               \
                          m_MinValue.value<TYPE>( ),   \
                          m_MaxValue.value<TYPE>( ),   \
                          m_Decimal,                   \
                          m_Step );                    \
    }                                                  \

CLONE_DEF( CFilterInt, int )
CLONE_NUMERIC_DEF( CFilterFloat, float )
CLONE_NUMERIC_DEF( CFilterDouble, double )
CLONE_DEF( CFilterString, QString )
CLONE_DEF( CFilterChar, char )
CLONE_DEF( CFilterBool, bool )

#undef CLONE_DEF
#undef CLONE_NUMERIC_DEF

CFilterParameterMap::CFilterParameterMap( void )
{ }

CFilterParameterMap::CFilterParameterMap( const CFilterParameterMap &Params )
{
    CstIterator It = Params.Begin( );
    CstIterator CEnd = Params.End( );

    for( ; It != CEnd; ++It )
    {
        AFilterParameter *NewParam = (*It)->Clone( );
        AddParameter( NewParam );
    }

    // Copy the toggled list
    m_ToggledParams = Params.m_ToggledParams;
}

CFilterParameterMap::~CFilterParameterMap( void )
{
    Iterator It  = this->Begin( );
    Iterator End = this->End( );

    for( ; It != End; ++It )
    {
        delete (*It);
    }
    m_ParamList.clear( );

    m_ToggledParams.clear( );
}

void CFilterParameterMap::AddParameter( AFilterParameter *Param )
{
    QString Name = Param->GetName( );
    if( !Contains( Name ) )
    {
        m_ParamList.push_back( Param );
    }
}

void CFilterParameterMap::RemoveParameter( const QString &Name )
{
    Iterator It = Find( Name );
    if( It != End( ) )
    {
        delete (*It);
        m_ParamList.erase( It );
    }
}

const AFilterParameter* const CFilterParameterMap::GetParameter( const QString &Name ) const
{
    CstIterator It = Find( Name );
    if( It != End( ) )
    {
        return (*It);
    }
    return 0;
}

AFilterParameter* CFilterParameterMap::GetParameter( const QString &Name )
{
    Iterator It = Find( Name );
    if( It != End( ) )
    {
        return (*It);
    }
    return 0;
}

CFilterParameterMap& CFilterParameterMap::operator =( const CFilterParameterMap &Params )
{
    // Clear the current map first
    Iterator It = this->Begin( );
    Iterator IEnd = this->End( );
    for( ; It != IEnd; ++It )
    {
        delete (*It);
    }
    m_ParamList.clear( );

    // Copy the new param list
    CstIterator NIt = Params.Begin( );
    CstIterator NEnd = Params.End( );

    for( ; NIt != NEnd; ++It )
    {
        AFilterParameter *NewParam = (*NIt)->Clone( );
        AddParameter( NewParam );
    }

    // Copy the toggled list
    m_ToggledParams = Params.m_ToggledParams;

    return *this;
}

bool CFilterParameterMap::Contains( const QString &Name ) const
{
    CstIterator It = Find( Name );
    CstIterator End = this->End( );

    if( It != End )
        return true;

    return false;
}

CFilterParameterMap::Iterator CFilterParameterMap::Find( const QString &Name )
{
    Iterator It = this->Begin( );
    Iterator End = this->End( );

    for( ; It != End; ++It )
    {
        if( (*It)->GetName( ) == Name )
            break;
    }

    return It;
}

CFilterParameterMap::CstIterator CFilterParameterMap::Find( const QString &Name ) const
{
    CstIterator It = this->Begin( );
    CstIterator End = this->End( );

    for( ; It != End; ++It )
    {
        if( (*It)->GetName( ) == Name )
            break;
    }

    return It;
}

void CFilterParameterMap::AddToggledParameter( const QString &ParamName, const QString &TogglerName )
{
    m_ToggledParams.push_back( QPair<QString, QString>( ParamName, TogglerName ) );
}

bool CFilterParameterMap::IsToggledParameter( const QString &ParamName ) const
{
    ToggleCstIterator It = m_ToggledParams.begin( );
    ToggleCstIterator End = m_ToggledParams.end( );

    // The toggled parameter is always the first in a pair
    for( ; It != End; ++It )
    {
        if( It->first == ParamName )
        {
            return true;
        }
    }

    return false;
}

const QString& CFilterParameterMap::GetTogglingParameter( const QString &ToggledParamName ) const
{
    ToggleCstIterator It = m_ToggledParams.begin( );
    ToggleCstIterator End = m_ToggledParams.end( );

    // The toggling parameter is always the second in a pair
    for( ; It != End; ++It )
    {
        if( It->first == ToggledParamName )
        {
            break;
        }
    }

    if( It != End )
    {
        return It->second;
    }
    else
    {
		throw std::exception("ToggledParameter Not Found");
    }
}
