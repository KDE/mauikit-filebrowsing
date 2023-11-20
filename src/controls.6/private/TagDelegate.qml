import org.mauikit.controls 1.3 as Maui

/**
 * @private
 */
Maui.Chip
{
    id: control

    signal removeTag(int index)
    
    Maui.Theme.backgroundColor: model.color ? model.color : Qt.darker(Maui.Theme.backgroundColor, 1.1)
    
    onClose: removeTag(index)
    label.text: model.tag       
}
