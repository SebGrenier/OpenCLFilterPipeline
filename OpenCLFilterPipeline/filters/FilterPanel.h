#ifndef __PANELS_FILTERPANEL_H__
#define __PANELS_FILTERPANEL_H__

#include "BaseFilter.h"
#include "FilterParameter.h"

#include <QGroupBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QMap>
#include <QPushButton>
#include <QCheckBox>
#include <QSignalMapper>

class CFilterPanel : public QGroupBox
{
    Q_OBJECT
public:
    explicit CFilterPanel( QWidget *Parent, CFilterParameterMap *Params, const QString &FilterName );
    ~CFilterPanel( void );

    inline const QString& GetName( void ) const { return m_Name; }

    void ResetFilterParameters( void );

private slots:
    void on_ResetClicked( void );
    void on_ToggleClicked( QWidget* Widget );
	void on_SpinBoxIntChanged(int value);
	void on_SpinBoxDoubleChanged(double value);

private:
	void on_WidgetChanged(QWidget* Widget);

private:
    CFilterPanel( void ){ }
    void BuildPanel( void );
    void AddWidget( QWidget *Widget, const QString &ParamName, const QString &Description );
    void CheckAndLinkToggle( QWidget *Widget, const QString &ParamName );

    QString m_Name;

    CFilterParameterMap             *m_Params;
    QFormLayout                    *m_ParamLayout;
    QMap< QWidget*, QString >       m_WidgetList;
    QMultiMap< QCheckBox*, QWidget* > m_LinkedWidgetList;

    QHBoxLayout   *m_ButtonLayout;
    QPushButton   *m_ResetButton;
    QSignalMapper *m_SignalMapper;
};

#endif // __PANELS_FILTERPANEL_H__
