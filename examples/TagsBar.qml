import QtQuick
 import QtCore
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

            FB.FileBrowser
            {
                id: _fileBrowser
                anchors.fill: parent
                currentPath: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
                settings.viewType: FB.FMList.LIST_VIEW
                onItemClicked: (index) => _tagPreviewPage.fileItem = _fileBrowser.currentFMModel.get(index)
            }
        }

        Maui.Page
        {
            id: _tagPreviewPage
            Maui.Controls.showCSD: true
            anchors.fill: parent

            property var fileItem

            Maui.Holder
            {
                anchors.fill: parent
                emojiSize: 100
                imageSource: _tagPreviewPage.fileItem.thumbnail
                title: _tagPreviewPage.fileItem.name
            }

            footer: FB.TagsBar
            {
                list.strict: false
                list.urls: [_tagPreviewPage.fileItem.url]
                width: parent.width

                onTagsEdited: (tags) => list.updateToUrls(tags)
            }
        }
    }
}

