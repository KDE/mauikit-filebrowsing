import QtQuick 
import QtQuick.Layouts
import QtQuick.Controls 

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

/**
 * @inherit org::mauikit::controls::InputDialog
 * @brief An input dialog to create one or multiple new tags.
 *
 * This control inherits from MauiKit InputDialog, to checkout its inherited properties refer to docs.
 * 
 * @note Multiple tags can be created at once by separating the entries by a comma.
 * 
 *@image html newtagdialog.png
 *
 * @code
 * Maui.Page
 * {
 *    Maui.Controls.showCSD: true
 *    anchors.fill: parent
 * 
 *    Button
 *    {
 *        anchors.centerIn: parent
 *        text: "New tag"
 *        onClicked: _tagDialog.open()
 *    }
 * 
 *    FB.NewTagDialog
 *    {
 *        id: _tagDialog
 *    }
 * }
 * @endcode
 *
 * <a href="https://invent.kde.org/maui/mauikit-filebrowser/examples/NewTagDialog.qml">You can find a more complete example at this link.</a>
 */
Maui.InputDialog
{
    id: control
    
    /**
     * @brief The current colors picked for the new tags.
     * @property color NewTagDialog::currentColor
     */
    readonly property alias currentColor : _colorsRow.currentColor
    
    /**
     * @brief The list of the default colors used for the Tagging system.
     * `["#4DD0E1", "#9575CD", "#F06292", "#DCE775", "#FFD54F", "#FF8A65", "#90A4AE"]`
     */
    readonly property var defaultColors : ["#4DD0E1", "#9575CD", "#F06292", "#DCE775", "#FFD54F", "#FF8A65", "#90A4AE"]
    
    title: i18nd("mauikitfilebrowsing", "New tags")
    message: i18nd("mauikitfilebrowsing", "Create new tags to organize your files. You can create multiple tags separated by a comma.")
    
    textEntry.placeholderText: i18n("New tags")    
    
    Maui.ColorsRow
    {
        id: _colorsRow
        Layout.fillWidth: true
        
        colors: control.defaultColors
        onColorPicked: currentColor = color
    }
    
    Flow
    {
        Layout.fillWidth: true
        
        visible: control.textEntry.text.length
        spacing: Maui.Style.defaultSpacing    
        
        Repeater
        {
            model: textEntry.text.split(",")
            
            delegate: Maui.Chip
            {
                label.text: modelData
                showCloseButton: false
                color: control.currentColor
                iconSource: "tag"
            }
        }       
    }
    
    onFinished: (text) =>
    {
        for(var tag of text.split(","))
        {
            FB.Tagging.tag(tag, control.currentColor, "")
        }    
    }
    
    onClosed:
    {
        control.currentColor = ""
    }
}
