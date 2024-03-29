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

    function removeDuplicateBlobs(blobsWithDuplicates) {
        return Engine.removeDuplicatedBlobs(blobsWithDuplicates)
    }

    function runAsync() {
        asyncToGenerator( function* () {

            let blobs = DataBus.found_blobs3

            if (blobs === undefined) {
                appendLog("no blobs to visit\n")
                return
            }

            blobs = removeDuplicateBlobs(blobs)

            let updatedBlobs = []

            statusTimer.interval = 100
            statusTimer.start()

            // point - массив строк. Возвр значение строка с пробелом между координатами
            const updateBlobPosition = function* (point) {
                xTarget = Number(point[0])
                yTarget = Number(point[1])

                moveTo(xTarget, yTarget)

                yield waitForCondition(() => status === "Idle" &&
                                       Math.abs(xTarget - xPos) <= 0.003 &&
                                       Math.abs(yTarget - yPos) <= 0.003)

                appendLog("capturing ...\n")
                Video4.captureSmallRegion(5.5)
                yield waitForSignal(Video4.capturedSmallRegion)
                appendLog("captured\n")

                var smallRegion = Video4.smallRegion()
                OpenCv.blobDetectorUpdated(smallRegion)
                yield waitForSignal(OpenCv.smallRegionBlobChanged, 2000)

                let coordBlob = OpenCv.smallRegionBlob()

                if (coordBlob.length === 0) {
                    appendLog("blob NOT found\n")
                    return [false, point.join(" ")]
                }
                else {
                    appendLog("blob found\n")
                    return [true, coordBlob]
                }
            }

            const start = Date.now()

            for (let blob of blobs) {

                let point = blob.split(" ")
                let [ok1, foundPoint1] = yield* updateBlobPosition(point)

                let [ok2, foundPoint2] = yield* updateBlobPosition(foundPoint1.split(" "))

                if (ok2) {
                    let dist = distance(blob, foundPoint2)
                    updatedBlobs.push(foundPoint2 + " " + dist)
                }
                else {
                    updatedBlobs.push(blob += " NOK")
                }
            }

            const finish = Date.now()


            statusTimer.stop()
            updatedBlobs = updatedBlobs.sort( (a, b) => parseFloat(a.split(' ')[0]) >  parseFloat(b.split(' ')[0]) ? 1 : -1)
            removeDuplicateBlobs(updatedBlobs)
            appendLog("visit finished\n")
            appendLog("visit time " + Math.floor((finish-start)/1000) + " sec\n")
            appendLog("visit count " + blobs.length + "\n")

            DataBus.found_blobs3 = updatedBlobs
            DataBus.found_blobs4 = updatedBlobs.join('<br>')


        } )();
    }
}
