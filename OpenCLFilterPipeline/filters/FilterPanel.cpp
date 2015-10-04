#include "FilterPanel.h"
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTime>
#include <QVariant>


CFilterPanel::CFilterPanel( QWidget *Parent, CFilterParameterMap *Params, const QString &FilterName )
    : QGroupBox       ( Parent     )
    , m_Name          ( FilterName )
    , m_Params        ( Params     )
    , m_ParamLayout   ( 0          )
    , m_ButtonLayout  ( 0          )
    , m_ResetButton   ( 0          )
    , m_SignalMapper  ( 0          )
{
    m_ParamLayout = new QFormLayout( this );
    this->setTitle( m_Name );

    m_SignalMapper = new QSignalMapper( this );
    connect( m_SignalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_ToggleClicked(QWidget*)) );

    BuildPanel( );

    // Build the layout for the push buttons
    m_ButtonLayout = new QHBoxLayout( 0 );

    // Build the reset button
    m_ResetButton = new QPushButton( 0 );
    m_ResetButton->setText( "Reset" );

    m_ButtonLayout->addWidget( m_ResetButton );

    m_ParamLayout->addRow( m_ButtonLayout );

    connect( m_ResetButton, SIGNAL( clicked() ), this, SLOT( on_ResetClicked() ) );
}

CFilterPanel::~CFilterPanel( void )
{
    if( m_SignalMapper )
        delete m_SignalMapper;
}

void CFilterPanel::BuildPanel( void )
{
    CFilterParameterMap::Iterator It = m_Params->Begin( );
    CFilterParameterMap::Iterator End = m_Params->End( );

    QString ParamName;
    QString Description;
    AFilterParameter *Param = 0;
    CFilterDouble *DoubleParam = 0;
    CFilterFloat *FloatParam = 0;
    QSpinBox *WidgetInt = 0;
    QDoubleSpinBox *WidgetDouble = 0;
    QCheckBox *WidgetBool = 0;
    for( ; It != End; ++It )
    {
        Param = (*It);
        ParamName = Param->GetName( );
        Description = Param->GetDescription( );

        // Create the appropriate widget
        switch( Param->GetType( ) )
        {
        case AFilterParameter::FP_INT :
            WidgetInt = new QSpinBox( 0 );
            WidgetInt->setValue( CFilterParameterInterpreter< int >::Convert( Param ) );
            WidgetInt->setMinimum( CFilterParameterInterpreter< int >::Convert( Param->GetMin( ) ) );
            WidgetInt->setMaximum( CFilterParameterInterpreter< int >::Convert( Param->GetMax( ) ) );
            AddWidget( WidgetInt, ParamName, Description );
            CheckAndLinkToggle( WidgetInt, ParamName );
            break;
        case AFilterParameter::FP_FLOAT :
            FloatParam = static_cast< CFilterFloat* >( Param );
            WidgetDouble = new QDoubleSpinBox( 0 );
            WidgetDouble->setValue( CFilterParameterInterpreter< double >::Convert( Param ) );
            WidgetDouble->setDecimals( FloatParam->GetDecimal( ) );
            WidgetDouble->setSingleStep( (double)FloatParam->GetStep( ) );
            WidgetDouble->setMinimum( CFilterParameterInterpreter< double >::Convert( Param->GetMin( ) ) );
            WidgetDouble->setMaximum( CFilterParameterInterpreter< double >::Convert( Param->GetMax( ) ) );
            AddWidget( WidgetDouble, ParamName, Description );
            CheckAndLinkToggle( WidgetDouble, ParamName );
            break;
        case AFilterParameter::FP_DOUBLE :
            DoubleParam = static_cast< CFilterDouble* >( Param );
            WidgetDouble = new QDoubleSpinBox( 0 );
            WidgetDouble->setValue( CFilterParameterInterpreter< double >::Convert( Param ) );
            WidgetDouble->setDecimals( DoubleParam->GetDecimal( ) );
            WidgetDouble->setSingleStep( DoubleParam->GetStep( ) );
            WidgetDouble->setMinimum( CFilterParameterInterpreter< double >::Convert( Param->GetMin( ) ) );
            WidgetDouble->setMaximum( CFilterParameterInterpreter< double >::Convert( Param->GetMax( ) ) );
            AddWidget( WidgetDouble, ParamName, Description );
            CheckAndLinkToggle( WidgetDouble, ParamName );
            break;
        case AFilterParameter::FP_BOOL :
            WidgetBool = new QCheckBox( 0 );
            WidgetBool->setChecked( CFilterParameterInterpreter< bool >::Convert( Param ) );
            AddWidget( WidgetBool, ParamName, Description );
            CheckAndLinkToggle( WidgetBool, ParamName );

            // Connect the signal to the slot with the signal mapper
            m_SignalMapper->setMapping( WidgetBool, WidgetBool );
            connect( WidgetBool, SIGNAL(stateChanged(int)), m_SignalMapper, SLOT(map()) );
        default :
            break;
        }
    }
}

void CFilterPanel::AddWidget( QWidget *Widget,
                             const QString &ParamName,
                             const QString &Description )
{
    Widget->setToolTip( Description );
    m_WidgetList.insert( Widget, ParamName );
    m_ParamLayout->addRow( ParamName, Widget );
	connect(Widget, SIGNAL(changed(QWidget*)), this, SLOT(on_WidgetChanged(QWidget*)));
}

void CFilterPanel::ResetFilterParameters( void )
{
    QMap< QWidget*, QString >::iterator It = m_WidgetList.begin( );
    QMap< QWidget*, QString >::iterator End = m_WidgetList.end( );

    AFilterParameter *Param = 0;
    QSpinBox *WidgetInt = 0;
    QDoubleSpinBox *WidgetDouble = 0;
    QCheckBox *WidgetBool = 0;
    for( ; It != End; ++It )
    {
        Param = m_Params->GetParameter( It.value( ) );
        if( Param )
        {
            // Get the widget value based on the parameter type
            switch( Param->GetType( ) )
            {
            case AFilterParameter::FP_INT :
                WidgetInt = static_cast<QSpinBox*>( It.key( ) );
                if( WidgetInt )
                    WidgetInt->setValue( CFilterParameterInterpreter< int >::Convert( Param->GetDefaultValue( ) ) );
                break;
            case AFilterParameter::FP_FLOAT :
                WidgetDouble = static_cast<QDoubleSpinBox*>( It.key( ) );
                if( WidgetDouble )
                    WidgetDouble->setValue( CFilterParameterInterpreter< double >::Convert( Param->GetDefaultValue( ) ) );
                break;
            case AFilterParameter::FP_DOUBLE :
                WidgetDouble = static_cast<QDoubleSpinBox*>( It.key( ) );
                if( WidgetDouble )
                    WidgetDouble->setValue( CFilterParameterInterpreter< double >::Convert( Param->GetDefaultValue( ) ) );
                break;
            case AFilterParameter::FP_BOOL :
                WidgetBool = static_cast<QCheckBox*>( It.key( ) );
                if( WidgetBool )
                    WidgetBool->setChecked( CFilterParameterInterpreter< bool >::Convert( Param->GetDefaultValue( ) ) );
            default :
                break;
            }
        }
    }
}

void CFilterPanel::on_ResetClicked( void )
{
    ResetFilterParameters( );
}

void CFilterPanel::CheckAndLinkToggle( QWidget *Widget, const QString &ParamName )
{
    if( m_Params->IsToggledParameter( ParamName ) )
    {
        // If the parameter is toggleable, get the toggling parameter
        const QString TogglingParamName = m_Params->GetTogglingParameter( ParamName );

        // Get the widget associated with the toggling parameter
        QMap< QWidget*, QString >::iterator It = m_WidgetList.begin( );
        QMap< QWidget*, QString >::iterator End = m_WidgetList.end( );

        for( ; It != End; ++It )
        {
            if( It.value( ) == TogglingParamName )
            {
                // Check if the toggling widget is a QCheckBox
                QCheckBox *WidgetBool = static_cast<QCheckBox*>( It.key( ) );
                if( WidgetBool )
                {
                    // Insert the link into the list
                    m_LinkedWidgetList.insert( WidgetBool, Widget );

                    // Set the state of the toggleable widget
                    Widget->setEnabled( WidgetBool->isChecked( ) );
                }
            }
        }
    }
}

void CFilterPanel::on_ToggleClicked( QWidget *Widget )
{
    QCheckBox *WidgetBool = static_cast<QCheckBox*>( Widget );
    if( WidgetBool == 0 )
        return;

    QList< QWidget* > ToggledWidgetList = m_LinkedWidgetList.values( WidgetBool );
    QList< QWidget* >::const_iterator It = ToggledWidgetList.begin( );
    QList< QWidget* >::const_iterator End = ToggledWidgetList.end( );
    for( ; It != End; ++It )
    {
        (*It)->setEnabled( WidgetBool->isChecked( ) );
    }
}

void CFilterPanel::on_WidgetChanged(QWidget* Widget)
{
	if (!m_WidgetList.contains(Widget))
		return;

	AFilterParameter *Param = 0;
	QSpinBox *WidgetInt = 0;
	QDoubleSpinBox *WidgetDouble = 0;
	QCheckBox *WidgetBool = 0;
	Param = m_Params->GetParameter(m_WidgetList[Widget]);
	if (Param)
	{
		// Get the widget value based on the parameter type
		switch (Param->GetType())
		{
		case AFilterParameter::FP_INT:
			WidgetInt = static_cast<QSpinBox*>(Widget);
			if (WidgetInt)
				CFilterParameterInterpreter<int>::Assign(Param, WidgetInt->value());
			break;
		case AFilterParameter::FP_FLOAT:
			WidgetDouble = static_cast<QDoubleSpinBox*>(Widget);
			if (WidgetDouble)
				CFilterParameterInterpreter<float>::Assign(Param, WidgetDouble->value());
			break;
		case AFilterParameter::FP_DOUBLE:
			WidgetDouble = static_cast<QDoubleSpinBox*>(Widget);
			if (WidgetDouble)
				CFilterParameterInterpreter<double>::Assign(Param, WidgetDouble->value());
			break;
		case AFilterParameter::FP_BOOL:
			WidgetBool = static_cast<QCheckBox*>(Widget);
			if (WidgetBool)
				CFilterParameterInterpreter<bool>::Assign(Param, WidgetBool->isChecked());
		default:
			break;
		}
	}
}