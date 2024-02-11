import QtQuick 2.12
import QtWebEngine 1.3
import QtQuick.Layouts 1.11

WebEngineView {
    id: webView
    url: initialUrl

    onNewViewRequested: function(request) {
        request.openIn(webView)
    }
}