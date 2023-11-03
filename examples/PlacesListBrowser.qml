import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

Maui.ApplicationWindow
{
    id: root

    Maui.SideBarView
    {
        anchors.fill: parent

        sideBar.content: Pane
        {
            Maui.Theme.colorSet: Maui.Theme.Window
            anchors.fill: parent
            FB.PlacesListBrowser
            {
                anchors.fill: parent
            }
        }

        Maui.Page
        {
            Maui.Controls.showCSD: true
            anchors.fill: parent
        }
    }
}

