#include "StateMachineWidgetPlugin.hpp"
#include "StateMachineWidget.hpp"

Q_EXPORT_PLUGIN2(StateMachineWidget, StateMachineWidgetPlugin)

StateMachineWidgetPlugin::StateMachineWidgetPlugin(QObject *parent)
    : QObject(parent)
{   
    initialized = false;
}

StateMachineWidgetPlugin::~StateMachineWidgetPlugin()
{
}

bool StateMachineWidgetPlugin::isContainer() const
{
    return false;
}

bool StateMachineWidgetPlugin::isInitialized() const
{
    return initialized;
}

QIcon StateMachineWidgetPlugin::icon() const
{
    return QIcon("");
}

QString StateMachineWidgetPlugin::domXml() const
{
        return "<ui language=\"c++\">\n"
            " <widget class=\"StateMachineWidget\" name=\"statemachinewidget\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "     <width>300</width>\n"
            "     <height>120</height>\n"
            "   </rect>\n"
            "  </property>\n"
//            "  <property name=\"toolTip\" >\n"
//            "   <string>StateMachineWidget</string>\n"
//            "  </property>\n"
//            "  <property name=\"whatsThis\" >\n"
//            "   <string>StateMachineWidget</string>\n"
//            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}

QString StateMachineWidgetPlugin::group() const {
    return "Rock-Robotics";
}

QString StateMachineWidgetPlugin::includeFile() const {
    return "StateMachineWidget/StateMachineWidget.hpp";
}

QString StateMachineWidgetPlugin::name() const {
    return "StateMachineWidget";
}

QString StateMachineWidgetPlugin::toolTip() const {
    return whatsThis();
}

QString StateMachineWidgetPlugin::whatsThis() const
{
    return "";
}

QWidget* StateMachineWidgetPlugin::createWidget(QWidget *parent)
{
    return new StateMachineWidget(parent);
}

void StateMachineWidgetPlugin::initialize(QDesignerFormEditorInterface *core)
{
     if (initialized)
         return;
     initialized = true;
}
