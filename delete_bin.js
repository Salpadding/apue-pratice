#!/usr/bin/env node

/**
 * delete binary file generated by make command
 */

const fs = require('fs')
const path = require('path')

for(let sub of fs.readdirSync(__dirname)) {
    if(!fs.statSync(sub).isDirectory())
        continue
    if(sub == '.git')
        continue

    const srcs = fs.readdirSync(sub)
    for(let f of srcs) {
        if(!f.endsWith('.c')) {
            fs.unlinkSync(path.join(__dirname, sub, f))
        }
    }
}
