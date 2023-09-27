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


function generateSteps(x0, y0, xMax, yMax, stepX, stepY, feed) {
    var x = x0
    var y = y0

    var cmds = []

    while (x < xMax) {
        while (y < yMax) {
            cmds.push("G1 G90 F" + feed + " X" + x + " Y" + y)
            y += stepY
        }
        x += stepX
        y = y0
    }

    return cmds
}
