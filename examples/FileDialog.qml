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

        Column
        {
            width: 100
            anchors.centerIn: parent

            Button
            {
                text: "Open"
                onClicked:
                {
                    _dialog.mode = FB.FileDialog.Modes.Open
                    _dialog.callback = (paths) =>
                            {
                        console.log("Selected Paths", paths)
                        _text.text = paths.join("\n")
                    }
                    _dialog.open()
                }
            }

            Button
            {
                text: "Save"
                onClicked:
                {
                    _dialog.mode = FB.FileDialog.Modes.Save
                    _dialog.callback = (paths) =>
                            {
                        console.log("Save to", paths)
                        _text.text = paths.join("\n")
                    }
                    _dialog.open()
                }
            }

            Text
            {
                id: _text
            }
        }
    }

    FB.FileDialog
    {
        id: _dialog
    }
}

