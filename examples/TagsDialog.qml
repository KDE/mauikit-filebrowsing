import QtQuick
import QtCore
import QtQuick.Controls

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

Maui.ApplicationWindow
{
    id: root

    Maui.Page
    {
        id: _tagPreviewPage
        Maui.Controls.showCSD: true
        anchors.fill: parent

        title: "Add tags to a file"

        FB.FileBrowser
        {
            id: _fileBrowser
            anchors.fill: parent
            currentPath: StandardPaths.writableLocation(StandardPaths.DownloadLocation)
            settings.viewType: FB.FMList.LIST_VIEW
            onItemClicked: (index) =>
                           {
                               _tagsDialog.composerList.urls = [_fileBrowser.currentFMModel.get(index).url]
                               _tagsDialog.open()
                           }
        }

        FB.TagsDialog
        {
            id: _tagsDialog
            composerList.strict: false //Show all the associated tags to the file
            onTagsReady: (tags) => console.log(tags)
        }
    }

}

