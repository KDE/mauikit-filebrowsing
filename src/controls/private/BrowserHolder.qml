import QtQuick 2.9
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
    property bool visible: (browser.status.code === FB.PathStatus.ERROR || ( browser.status.empty) || !browser.status.exists) 

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
}
