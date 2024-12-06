import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

ToolButton
{
    id: control

    property string url

    readonly property alias isFav : _private.isFav

    checked: isFav

    icon.name: "love"
    icon.color: mcolor
    property color mcolor : (control.checked ? "white" :  (control.pressed || control.down || control.highlighted || control.hovered ) ?Maui.Theme.highlitedTextColor : Maui.Theme.textColor)
    
    Behavior on mcolor
    {
        Maui.ColorTransition{}
    }

    QtObject
    {
        id: _private
        property bool isFav

        Binding on isFav
        {
            value: control.url.length > 0 ? FB.Tagging.isFav(control.url) : false
            restoreMode: Binding.RestoreBindingOrValue
            delayed: true
        }
    }

    onClicked:
    {
        if(!checked)
            xAnim.restart()

        FB.Tagging.toggleFav(control.url)
        _private.isFav = FB.Tagging.isFav(control.url)
    }

    SequentialAnimation on scale
    {
        id: xAnim
        // Animations on properties start running by default
        running: false
        loops: 3
        NumberAnimation { from: 1; to: 0.95; duration: 100; easing.type: Easing.InBack }
        NumberAnimation { from: 0.95; to: 1; duration: 100; easing.type: Easing.InBack }
        PauseAnimation { duration: 40 } // This puts a bit of time between the loop
    }

    background: Rectangle
    {
       radius:  Maui.Style.radiusV

        color: control.pressed || control.down || control.highlighted || control.hovered ? Maui.Theme.hoverColor : "transparent"

        Rectangle
        {
            anchors.fill: parent
            color: "#f84172"
            // visible: control.checked
            scale: control.checked ? 1 : 0
            radius: control.checked ? Maui.Style.radiusV : control.height
            opacity: control.checked ? 1 : 0
            Behavior on radius
            {
                NumberAnimation
                {
                    duration: Maui.Style.units.longDuration
                    easing.type: Easing.InQuad
                }
            }

            Behavior on scale
            {
                NumberAnimation
                {
                    duration: Maui.Style.units.longDuration
                    easing.type: Easing.InQuad
                }
            }

            Behavior on opacity
            {
                NumberAnimation
                {
                    duration: Maui.Style.units.shortDuration
                    easing.type: Easing.InQuad
                }
            }
        }


    }
}
