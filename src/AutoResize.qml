import QtQuick 2.0

Item {
    id:globalResize
    property var targetItem: parent
    property bool fixedAspectRatio: false // Else zoom from width and height
    property bool accordingToX: true // Else according to center
    property bool fontAccordingToMax: false
    property bool ifAutoResize: true
    property string ingnoreAll: "ingnoreAll"
    property string ingnoreChildren: "ingnoreChildren"
    //变换比例
    property real horizontalRatio: 1.0
    property real verticalRatio: 1.0
    property real fontRatio: 1.0

    property var targetItemGeometry
    property var childrenItemGeometry
    property var childrenText
    property real fontSizeScaleFactor: 1.0
    property bool isBegin: false
    signal resized()
    function begin() {
        var _childrenItemGeometry=new Array;
        targetItemGeometry = new Object;
        targetItemGeometry["width"] = targetItem.width;
        targetItemGeometry["height"] = targetItem.height;
        var children = targetItem.children;
        for(var index = 1; index < children.length; index++)
        {
            var currentItem = children[index];
            var buf = new Object;

            buf["item"] = currentItem;
            buf["name"]=currentItem.objectName;
            buf["x"] = currentItem.x;
            buf["y"] = currentItem.y;
            buf["centerX"] = currentItem.x + (currentItem.width / 2);
            buf["centerY"] = currentItem.y + (currentItem.height / 2);
            buf["width"] = currentItem.width;
            buf["height"] = currentItem.height;

            //to salce the font size
            buf["fontSize"]=0;
            if(currentItem.font!=undefined)
            {
                buf["fontSize"]=currentItem.font.pointSize
            }
            //跳过当前对象以及所有子对象
            if(buf["name"]==ingnoreAll)
            {
                continue;
            }
            //跳过当前对象的子对象
            else if(buf["name"]==ingnoreChildren)
            {
                _childrenItemGeometry.push(buf)
            }
            else
            {
                _childrenItemGeometry.push(buf)
                getAllChildren(_childrenItemGeometry,currentItem)
            }
        }
        childrenItemGeometry=_childrenItemGeometry
        //console.log("length->"+_childrenItemGeometry.length)
        isBegin = true;
    }
    function getAllChildren(_childrenItemGeometry,target)
    {
        var children = target.children;
        for(var index = 0; index < children.length; index++)
        {
            var currentItem = children[index];
            var buf = new Object;

            buf["item"] = currentItem;
            buf["name"]=currentItem.objectName;
            buf["x"] = currentItem.x;
            buf["y"] = currentItem.y;
            buf["centerX"] = currentItem.x + (currentItem.width / 2);
            buf["centerY"] = currentItem.y + (currentItem.height / 2);
            buf["width"] = currentItem.width;
            buf["height"] = currentItem.height;
            buf["fontSize"]=0;
            if(currentItem.font!=undefined)
            {
                buf["fontSize"]=currentItem.font.pointSize
            }
            //跳过当前对象以及所有子对象
            if(buf["name"]=="ingnoreAll")
            {
                continue;
            }
            //跳过当前对象的子对象
            else if(buf["name"]=="ingnoreChildren")
            {
                _childrenItemGeometry.push(buf)
            }
            else
            {
                _childrenItemGeometry.push(buf)
                getAllChildren(_childrenItemGeometry,currentItem)
            }
        }
    }

    function resize() {
        if(isBegin&&ifAutoResize)
        {
            //var horizontalRatio, verticalRatio,fontRatio;

            horizontalRatio = targetItem.width / targetItemGeometry["width"];
            verticalRatio = targetItem.height / targetItemGeometry["height"];
            fontRatio=horizontalRatio>verticalRatio?verticalRatio:horizontalRatio;
            //set fontRatio the greatest radio
            if(fontAccordingToMax)
                fontRatio=(horizontalCenter+verticalRatio)-fontRatio
            //console.log("radio->"+horizontalRatio+"--"+verticalRatio)
            //console.log("toal item ->"+childrenItemGeometry.length)
            for(var index = 0; index < childrenItemGeometry.length; index++)
            {
                var currentItem=childrenItemGeometry[index]
                if(fixedAspectRatio)
                {
                    if(horizontalRatio > verticalRatio)
                    {
                        childrenItemGeometry[index]["item"].width  = childrenItemGeometry[index]["width"] * verticalRatio;
                        childrenItemGeometry[index]["item"].height = childrenItemGeometry[index]["height"] * verticalRatio;
                    }
                    else
                    {
                        childrenItemGeometry[index]["item"].width  = childrenItemGeometry[index]["width"] * horizontalRatio;
                        childrenItemGeometry[index]["item"].height = childrenItemGeometry[index]["height"] * horizontalRatio;
                    }
                }
                else
                {
                    childrenItemGeometry[index]["item"].width  = childrenItemGeometry[index]["width"] * horizontalRatio;
                    childrenItemGeometry[index]["item"].height = childrenItemGeometry[index]["height"] * verticalRatio;
                }
                if(accordingToX)
                {
                    childrenItemGeometry[index]["item"].x = childrenItemGeometry[index]["x"] * horizontalRatio;
                    childrenItemGeometry[index]["item"].y = childrenItemGeometry[index]["y"] * verticalRatio;
                }
                else
                {
                    childrenItemGeometry[index]["item"].x = childrenItemGeometry[index]["centerX"] * horizontalRatio - (childrenItemGeometry[index]["item"].width / 2);
                    childrenItemGeometry[index]["item"].y = childrenItemGeometry[index]["centerY"] * verticalRatio - (childrenItemGeometry[index]["item"].height / 2);
                }
                if(childrenItemGeometry[index]["item"].font!=undefined)
                {
                    childrenItemGeometry[index]["item"].font.pointSize = childrenItemGeometry[index]["fontSize"]*fontRatio*fontSizeScaleFactor
                }
            }
            globalResize.resized();

        }
    }

    Component.onCompleted: {
        begin();
    }

    Component {
        id: connections

        Connections {
            target: targetItem

            onWidthChanged: {
                resize();
            }
            onHeightChanged:
            {
                resize();
            }
        }
    }

    Loader {
        Component.onCompleted: {
            sourceComponent = connections;
        }
    }
}
