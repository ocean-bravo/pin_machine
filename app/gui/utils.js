String.prototype.log = function() {
    var args = Array.prototype.slice.call(arguments);
    var rep= args.slice(0, args.length);
    var i=0;
    var output = this.replace(/%s|%d|%f|%@/g, function(match,idx) {
        var subst=rep.slice(i, ++i);
        return( subst );
    });
    return output;
}


function generateSteps(xMin, yMin, xMax, yMax, stepX, stepY, feed) {
    var x = xMin
    var y = yMin

    var cmds = []

    while (x < xMax) {
        while (y < yMax) {
            cmds.push("G1 G90 F" + feed + " X" + x + " Y" + y)
            y += stepY
        }
        cmds.push("G1 G90 F" + feed + " X" + x + " Y" + y)
        x += stepX
        while (y > yMin) {
            cmds.push("G1 G90 F" + feed + " X" + x + " Y" + y)
            y -= stepY
        }
        cmds.push("G1 G90 F" + feed + " X" + x + " Y" + y)
        x += stepX
        y = yMin
    }

    return cmds
}
