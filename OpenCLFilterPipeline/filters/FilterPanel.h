#ifndef __PANELS_FILTERPANEL_H__
#define __PANELS_FILTERPANEL_H__

#include "../Filters/AFilter.h"
#include "../Filters/FilterParameter.h"

#include <Liv4d.h>
#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QCheckBox>
#include <QSignalMapper>

using namespace Liv4d::Core;

class CFilterPanel : public QGroupBox, public AObservable< CFilterPanel >
{
    Q_OBJECT
public:
    explicit CFilterPanel( QWidget *Parent, const CFilterParameterMap &Params, const QString &FilterName );
    ~CFilterPanel( void );

    const CFilterParameterMap& GetFilterParameter( void );

    inline const QString& GetName( void ) const { return m_Name; }

    void ResetFilterParameters( void );

private slots:
    void on_ApplyClicked( void );
    void on_ResetClicked( void );
    void on_ToggleClicked( QWidget* Widget );

private:
    CFilterPanel( void ){ }
    void BuildPanel( void );
    void AddWidget( QWidget *Widget, const QString &ParamName, const QString &Description );
    void CheckAndLinkToggle( QWidget *Widget, const QString &ParamName );

    QString m_Name;

    CFilterParameterMap             m_Params;
    QFormLayout                    *m_ParamLayout;
    QMap< QWidget*, QString >       m_WidgetList;
    QMultiMap< QCheckBox*, QWidget* > m_LinkedWidgetList;

    QHBoxLayout   *m_ButtonLayout;
    QPushButton   *m_ResetButton;
    QPushButton   *m_ActivateButton;
    QSignalMapper *m_SignalMapper;
};

#endif // __PANELS_FILTERPANEL_H__
