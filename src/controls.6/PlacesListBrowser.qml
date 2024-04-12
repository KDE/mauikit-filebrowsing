import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

/**
 * @inherit org::mauikit::controls::ListBrowser
 * @brief A browsing list of the system locations, such as common standard places, bookmarks and others as removable devices and networks.
 * 
 * This control inherits from MauiKit ListBrowser, to checkout its inherited properties refer to docs.
 * 
 * Most of the properties to control the behaviour is handled via the PlacesList model, which is exposed via the `list` property.
 * @see list
 * 
 * @image html placeslistbrowser.png
 *
 * @code
 * Maui.SideBarView
 * {
 *    anchors.fill: parent
 * 
 *    sideBar.content: Pane
 *    {
 *        Maui.Theme.colorSet: Maui.Theme.Window
 *        anchors.fill: parent
 *        FB.PlacesListBrowser
 *        {
 *            anchors.fill: parent
 *        }
 *    }
 * 
 *    Maui.Page
 *    {
 *        Maui.Controls.showCSD: true
 *        anchors.fill: parent
 *    }
 * }
 *  @endcode
 * 
 * <a href="https://invent.kde.org/maui/mauikit-filebrowser/examples/PlacesListBrowser.qml">You can find a more complete example at this link.</a>
 */
Maui.ListBrowser
{
    id: control
    
    /**
     * @brief The model list of the places.
     * @property PlacesList PlacesListBrowser::list
     */
    readonly property alias list : placesList
    
    /**
     * @brief The contextual menu for the entries.
     * @note The menu has an extra property `index`, which refers to the index position of the entry where the menu was invoked at.
     * 
     * To add more entries, use the `itemMenu.data` property, or append/push methods.
     * @property Menu PlacesListBrowser::itemMenu
     */
    readonly property alias itemMenu : _menu
    
    /**
     * @brief The preferred size of the icon for the places delegates.
     * By default this is set to `Style.iconSizes.small`
     * @see Style::iconSizes
     */
    property int iconSize : Maui.Style.iconSizes.small
    
    /**
     * @brief The path of the current place selected.
     */
    property string currentPath
    
    /**
     * @brief Emitted when a entry has been clicked.
     * @param path the URL path of the entry 
     */
    signal placeClicked (string path)
    
    Maui.Theme.colorSet: Maui.Theme.View
    Maui.Theme.inherit: false    
    
    focus: true
    model: Maui.BaseModel
    {
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
                flat: false
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
