import QtQuick 2.15
import QtWebEngine 1.3
import QtQuick.Layouts 1.11

WebEngineView {
    property string initialUrl

    id: webView
    url: initialUrl

    onNewViewRequested: function(request) {
        request.openIn(webView)
    }
}