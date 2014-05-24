var iso = require('./build/Release/binding.node')
var child = require('fs').readFileSync('child.js')

var start = Date.now()
iso.demo(child, 324, function(res){
  console.log('\nTime:', Date.now() - start)
  console.log(res)
})

setInterval(function(){
  process.stdout.write('.')
}, 100)
