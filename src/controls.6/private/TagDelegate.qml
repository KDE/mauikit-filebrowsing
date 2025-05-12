import org.mauikit.controls as Maui

/**
 * @private
 */
Maui.Chip
{
    id: control

    signal removeTag(int index)
        
    onClose: removeTag(index)
    label.text: model.tag       
}
