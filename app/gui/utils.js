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
            cmds.push("G1 G90 F" + feed + " X" + parseFloat(x.toFixed(3)) + " Y" + parseFloat(y.toFixed(3))) // чтобы убрать trailing zeroes
            y += stepY
        }
        cmds.push("G1 G90 F" + feed + " X" + parseFloat(x.toFixed(3)) + " Y" + parseFloat(y.toFixed(3)))
        x += stepX
        while (y > yMin) {
            cmds.push("G1 G90 F" + feed + " X" + parseFloat(x.toFixed(3)) + " Y" + parseFloat(y.toFixed(3)))
            y -= stepY
        }
        cmds.push("G1 G90 F" + feed + " X" + parseFloat(x.toFixed(3)) + " Y" + parseFloat(y.toFixed(3)))
        x += stepX
        y = yMin
    }

    return cmds
}

function range(start, end) {
  return Array.apply(0, Array(end - start + 1)).map((element, index) => index + start);
}
