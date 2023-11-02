import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

Maui.ApplicationWindow
{
    id: root

    Maui.Page
    {
        Maui.Controls.showCSD: true
        anchors.fill: parent
        floatingFooter: true

        headBar.leftContent: Maui.ToolActions
        {
            autoExclusive: true

            Action
            {
                icon.name: "view-list-icons"
                checked: _browser.settings.viewType === FB.FMList.ICON_VIEW
                onTriggered: _browser.settings.viewType = FB.FMList.ICON_VIEW
            }

            Action
            {
                icon.name: "view-list-details"
                checked: _browser.settings.viewType === FB.FMList.LIST_VIEW
                onTriggered: _browser.settings.viewType = FB.FMList.LIST_VIEW
            }
        }

        FB.FileBrowser
        {
            id: _browser
            Maui.Controls.showCSD: true

            anchors.fill: parent
            currentPath: FB.FM.homePath()
            settings.viewType: FB.FMList.GRID_VIEW
            selectionBar: _selectionBar

            onItemDoubleClicked:(index) => openItem(index)
        }

        footer: Maui.SelectionBar
        {
            id: _selectionBar
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width-(Maui.Style.space.medium*2), implicitWidth)
            maxListHeight: root.height - (Maui.Style.contentMargins*2)

            Action
            {
                icon.name: "love"
                onTriggered: console.log(_selectionBar.getSelectedUrisString())
            }

            Action
            {
                icon.name: "folder"
                onTriggered: console.log(_selectionBar.contains("0"))
            }

            Action
            {
                icon.name: "list-add"
            }

            onExitClicked: clear()
        }
    }
}

