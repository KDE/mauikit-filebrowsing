import QtQuick 
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

import "."

/**
 * @inherit org::mauikit::controls::ListBrowser
 * @brief The flickable element for listing horizontally the tag buttons in the TagsBar control. This control is a graphical representation and consumes the TagsList model.
 * 
 * @warning This control only exists as an implementation detail of TagsBar, and it is exposed to access its own properties.
 */
Maui.ListBrowser
{
  id: control
  
  clip: false
  
  orientation: ListView.Horizontal
  spacing: Maui.Style.defaultSpacing
  
  implicitHeight: Maui.Style.toolBarHeight
  horizontalScrollBarPolicy: ScrollBar.AlwaysOff
  snapMode: ListView.SnapOneItem
  verticalScrollBarPolicy: ScrollBar.AlwaysOff
  
  /**
   * @brief Text to be displayed in the bar when the list is empty.
   */
  property string placeholderText: i18nd("mauikitfilebrowsing", "Add tags...")
  
  /**
   * @brief An alias to the tags model and controller. This is exposed to fine tune all the listing properties of the tags.
   * @property TagsList TagList::list
   */
  readonly property alias list : _tagsList
  
  /**
   * @brief Whether to display the placeholder text.
   * By default this is set to `true`
   */
  property bool showPlaceHolder:  true
  
  /**
   * @brief Whether the tag elements should have a clos/dismiss button.
   * When this button is made visible and is clicked the `tagRemoved` signal is triggered.
   * @see tagRemoved
   */
  property bool showDeleteIcon: true
  
  /**
   * @brief Emitted when the close/dismiss button in the tag button is clicked.
   * @see showDeleteIcon
   * @param index the index position of the tag element in the list
   */
  signal tagRemoved(int index)
  
  /**
   * @brief Emitted when a tag button has been clicked.
   * @param index the index position of the tag element in the list
   */
  signal tagClicked(int index)
  
  model: Maui.BaseModel
  {
    id: _tagsModel
    list: FB.TagsListModel
    {
      id: _tagsList
    }
  }
  
  Loader
  {
    anchors.fill: parent
    anchors.leftMargin: Maui.Style.space.medium
    asynchronous: true
    active: count === 0 && control.showPlaceHolder
    visible: active
    
    sourceComponent: Label
    {      
      verticalAlignment: Qt.AlignVCenter
      text: control.placeholderText
      opacity: 0.7
      color: Maui.Theme.textColor
    }
  }
  
  delegate: TagDelegate
  {
    showCloseButton: control.showDeleteIcon
    Maui.Theme.textColor: control.Maui.Theme.textColor
    
    ListView.onAdd:
    {
      control.flickable.positionViewAtEnd()
    }
    
    onRemoveTag: tagRemoved(index)
    onClicked: tagClicked(index)
  }    
}
