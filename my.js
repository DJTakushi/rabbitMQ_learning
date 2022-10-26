window.addEventListener("load", function() {
    var svgObject = document.getElementById('svg-object').contentDocument;
    console.log(svgObject)


    var startElement = document.getElementById('start');
    var endElement = document.getElementById('end');
    console.log(startElement)
    console.log(endElement)
    // New leader line has red color and size 8.
    // new LeaderLine(startElement, endElement, {color: 'red', size: 8});
});