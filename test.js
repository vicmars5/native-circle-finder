const { findCircles } = require('./build/Release/find-circles')
const { data, width, height } = require('./image.json')

const buffToCoordinates = (buff) => {
  const points = []
  for (let i = 0; i < buff.length; i+=8) {
    points.push({
      x: buff.readUIntBE(i, 4),
      y: buff.readUIntBE(i + 4, 4)
    })
  }
  return points
}

const buff = Buffer.from(data);
console.log('number', buff)

const xbuff = findCircles(buff, width, height, 255, 125)
const points = buffToCoordinates(xbuff)
console.log('points', points)
