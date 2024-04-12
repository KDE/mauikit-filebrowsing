import QtQuick 
import org.mauikit.filebrowsing as FB

QtObject
{
    /**
      *
      */
    property FB.FMList browser

    /**
      *
      */
    property bool visible: (browser.status.code === FB.PathStatus.ERROR || browser.status.empty) && browser.count <= 0


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
