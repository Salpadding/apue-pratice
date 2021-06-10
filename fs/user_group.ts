#!/usr/bin/env node -r ts-node/register

import fs = require('fs')

const passwdFile = '/etc/passwd'
const groupFile = '/etc/group'

interface User {
    name: string
    uid: number
    gid: number
    description: string
    home: string
    shell: string
}

interface Group {
    name: string,
    gid: number,
    users: string[]
}

function getGroups(): Map<number, Group> {
    const r = new Map<number, Group>()
    for(let line of fs.readFileSync(groupFile, 'utf-8').split('\n')) {
        if(line.startsWith('#'))
            continue
        if(line == '')
            continue
        const params = line.split(':')
        const g = {
            name: params[0],
            gid: parseInt(params[2]),
            users: 3 < params.length ? (params[3] == '' ? [] : params[3].split(',')) : []
        }
        r.set(g.gid, g)
    }
    return r
}

function getUsers(): Map<number, User> {
    const r = new Map<number, User>()
    for(let line of fs.readFileSync(passwdFile, 'utf-8').split('\n')) {
        if(line.startsWith('#'))
            continue
        if(line == '')
            continue
        const params = line.split(':')
        const u = {
            name: params[0],
            uid: parseInt(params[2]),
            gid: parseInt(params[3]),
            description: params[4],
            home: params[5],
            shell: params[6]
        }
        r.set(u.uid, u)
    }
    return r
}

const users = getUsers()
const groups = getGroups()
