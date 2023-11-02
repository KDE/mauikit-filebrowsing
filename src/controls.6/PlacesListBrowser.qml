import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.0 as FB

/**
 * PlacesListBrowser
 * A global sidebar for the application window that can be collapsed.
 *
 *
 *
 *
 *
 *
 */
Maui.ListBrowser
{
    id: control

    /**
      * list : PlacesList
      */
    property alias list : placesList

    /**
      * itemMenu : Menu
      */
    property alias itemMenu : _menu

    /**
      * iconSize : int
      */
    property int iconSize : Maui.Style.iconSizes.small
    
    property string currentPath

    /**
      * placeClicked :
      */
    signal placeClicked (string path)

    Maui.Theme.colorSet: Maui.Theme.View
    Maui.Theme.inherit: false
    
    
    focus: true
    model: Maui.BaseModel
    {
        id: placesModel
        list: FB.PlacesList
        {
            id: placesList
            groups: [
                FB.FMList.BOOKMARKS_PATH,
                FB.FMList.DRIVES_PATH]
        }
    }
    
                        currentIndex: placesList.indexOfPath(control.currentPath)


    section.property: "type"
    section.criteria: ViewSection.FullString
    section.delegate: Maui.LabelDelegate
    {
        id: delegate
        text: section

        isSection: true
        width: parent.width
        height: Maui.Style.toolBarHeightAlt
    }

    Maui.ContextualMenu
    {
        id: _menu
        property int index

        MenuItem
        {
            text: i18nd("mauikitfilebrowsing", "Edit")
        }

        MenuItem
        {
            text: i18nd("mauikitfilebrowsing", "Hide")
        }

        MenuItem
        {
            text: i18nd("mauikitfilebrowsing", "Remove")
            Maui.Theme.textColor: Maui.Theme.negativeTextColor
            onTriggered: list.removePlace(control.currentIndex)
        }
    }
    
    flickable.header: GridLayout
                    {
                        id: _quickSection
                        
                        width: Math.min(parent.width, 180)
                        rows: 3
                        columns: 3
                        columnSpacing: Maui.Style.space.small
                        rowSpacing: Maui.Style.space.small

                        Repeater
                        {
                        model: Maui.BaseModel
                        {
                            list: FB.PlacesList
                            {
                                id: _quickPacesList
                                groups: [FB.FMList.QUICK_PATH, FB.FMList.PLACES_PATH]
                            }
                        }

                        delegate: Maui.GridBrowserDelegate
                            {
                                Layout.preferredHeight: Math.min(50, width)
                                Layout.preferredWidth: 50
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                
                                isCurrentItem: control.currentPath === model.path
                                iconSource: model.icon +  (Qt.platform.os == "android" || Qt.platform.os == "osx" ? ("-sidebar") : "")
                                iconSizeHint: Maui.Style.iconSize
                                template.isMask: true
                                label1.text: model.label
                                labelsVisible: false
                                tooltipText: model.label
                                onClicked:
                                {
                                    placeClicked(model.path)
                                }
                            }
                        }
                        
                    }

    delegate: Maui.ListDelegate
    {
        width: ListView.view.width
        iconSize: control.iconSize
        labelVisible: true
        iconVisible: true
        label: model.label
        iconName: model.icon

        onClicked:
        {
            placeClicked(model.path)
        }

        onRightClicked:
        {
            _menu.index = index
            _menu.popup()
        }

        onPressAndHold:
        {
            _menu.index = index
            _menu.popup()
        }
    }
}
