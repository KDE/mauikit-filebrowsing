import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.7 as Kirigami
import org.kde.mauikit 1.0 as Maui
import org.mauikit.filebrowsing 1.0 as FB

QtObject
{
    /**
      *
      */
    property FB.FMList browser

    /**
      *
      */
    property bool visible: (browser.status.code === FB.FMList.LOADING || browser.status.code === FB.FMList.ERROR || ( browser.status.code === FB.FMList.READY && browser.status.empty === true)) && browser.count <= 0

    /**
      *
      */
    property string emoji: browser.status.icon

    /**
      *
      */
    property string title : browser.status.title

    /**
      *
      */
    property string body: browser.status.message

    /**
      *
      */
    property int emojiSize: Maui.Style.iconSizes.huge

}
