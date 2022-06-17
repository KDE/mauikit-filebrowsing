import org.mauikit.controls 1.3 as Maui

Maui.Chip
{
    id: control

    /**
     * 
     */
    property int tagHeight: Maui.Style.rowHeightAlt
    implicitHeight: tagHeight
    
    /**
     * 
     */
    signal removeTag(int index)
    
    Maui.Theme.backgroundColor: model.color ? model.color : Qt.darker(Maui.Theme.backgroundColor, 1.1)
    
    onClose: removeTag(index)
    label.text: model.tag   
    
}
