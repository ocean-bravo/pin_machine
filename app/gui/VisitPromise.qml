import QtQml 2.15
import QtQuick 2.15

QMLPromises {
    id: blobVisitorPromise

    property real xTarget
    property real yTarget

    function distance (a, b) {
        const [xa, ya] = a.split(' ')
        const [xb, yb] = b.split(' ')
        return (Number(xa) - Number(xb)).toFixed(3) + " " + (Number(ya) - Number(yb)).toFixed(3)
    }

    function runAsync() {
        asyncToGenerator( function* () {

            let blobs = DataBus.found_blobs3

            if (blobs === undefined) {
                appendLog("no blobs to visit\n")
                return
            }

            let updatedBlobs = []

            statusTimer.interval = 100
            statusTimer.start()

            for (let blob of blobs) {
                let point = blob.split(" ")

                moveTo(point[0], point[1])

                xTarget = Number(point[0])
                yTarget = Number(point[1])
                yield waitForCondition(() => root.status === "Idle" &&
                                       Math.abs(xTarget - xPos) <= 0.003 &&
                                       Math.abs(yTarget - yPos) <= 0.003)

                appendLog("capturing ...\n")
                Video4.captureSmallRegion()
                yield waitForSignal(Video4.capturedSmallRegion)
                appendLog("captured\n")

                var smallRegion = Video4.smallRegion()
                OpenCv.blobDetectorUpdated(smallRegion)
                yield waitForSignal(OpenCv.smallRegionBlobChanged, 2000)

                let coordBlob = OpenCv.smallRegionBlob()

                if (coordBlob.length === 0) {
                    appendLog("blob NOT found\n")
                    updatedBlobs.push(blob += " NOK")
                }
                else {
                    appendLog("blob found\n")

                    let dist = distance(blob, coordBlob)
                    updatedBlobs.push(coordBlob + " " + dist)
                }
            }

            statusTimer.stop()
            updatedBlobs = updatedBlobs.sort( (a, b) => parseFloat(a.split(' ')[0]) >  parseFloat(b.split(' ')[0]) ? 1 : -1)
            DataBus.found_blobs3 = updatedBlobs
            DataBus.found_blobs4 = updatedBlobs.join('<br>')

            appendLog("visit finished\n")
        } )();
    }
}
