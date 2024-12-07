"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[1615],{12949:(e,n,o)=>{o.r(n),o.d(n,{assets:()=>d,contentTitle:()=>i,default:()=>h,frontMatter:()=>s,metadata:()=>a,toc:()=>l});var r=o(74848),t=o(28453);const s={pagination_prev:null,pagination_next:null},i="Creating Dungeon Generation Algorithm",a={id:"guides/Best-Practices/Workflows/Dungeon-Generation-Algorithm",title:"Creating Dungeon Generation Algorithm",description:"If you struggle creating the dungeons you want, this page will help you getting a good basic workflow to design and code the dungeon you want to generate.",source:"@site/docs/guides/Best-Practices/Workflows/Dungeon-Generation-Algorithm.md",sourceDirName:"guides/Best-Practices/Workflows",slug:"/guides/Best-Practices/Workflows/Dungeon-Generation-Algorithm",permalink:"/ProceduralDungeon/guides/Best-Practices/Workflows/Dungeon-Generation-Algorithm",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},d={},l=[{value:"1. Defining what we want",id:"1-defining-what-we-want",level:2},{value:"2. Deducing the pieces of the puzzle",id:"2-deducing-the-pieces-of-the-puzzle",level:2},{value:"3. Deducing the constraints",id:"3-deducing-the-constraints",level:2},{value:"4. Writing pseudo-code",id:"4-writing-pseudo-code",level:2},{value:"Conclusion",id:"conclusion",level:2}];function c(e){const n={a:"a",admonition:"admonition",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",header:"header",img:"img",li:"li",p:"p",pre:"pre",ul:"ul",...(0,t.R)(),...e.components};return(0,r.jsxs)(r.Fragment,{children:[(0,r.jsx)(n.header,{children:(0,r.jsx)(n.h1,{id:"creating-dungeon-generation-algorithm",children:"Creating Dungeon Generation Algorithm"})}),"\n",(0,r.jsx)(n.p,{children:"If you struggle creating the dungeons you want, this page will help you getting a good basic workflow to design and code the dungeon you want to generate."}),"\n",(0,r.jsx)(n.admonition,{type:"info",children:(0,r.jsxs)(n.p,{children:["To illustrate the workflow, I'll use a simple example: a mall-like dungeon.",(0,r.jsx)(n.br,{}),"\n","This is only to help you understand the process behind creating your code based on a desired result.",(0,r.jsx)(n.br,{}),"\n","You can do anything else you want, you just have to follow the same steps."]})}),"\n",(0,r.jsxs)(n.admonition,{type:"tip",children:[(0,r.jsx)(n.p,{children:"Remember that you can do whatever you want!"}),(0,r.jsx)(n.p,{children:"You don't need an end room if you want another goal for your dungeon (e.g. killing all the mobs)."}),(0,r.jsx)(n.p,{children:"You don't need the player to spawn in the start room (or even have a specific room as a first room).\r\nYou could for example have multiple rooms with a player start actor you choose from to spawn the player."}),(0,r.jsx)(n.p,{children:"You can add any variables you want in your room data to add constraints on your generation rules (e.g. add a room difficulty value so you can choose a hard room after an easy room)."})]}),"\n",(0,r.jsx)(n.h2,{id:"1-defining-what-we-want",children:"1. Defining what we want"}),"\n",(0,r.jsx)(n.p,{children:"The first step is to write down the features of our dungeon and the end result we want to have."}),"\n",(0,r.jsx)(n.p,{children:"I want to make a mall-like dungeon.\r\nA mall is basically composed of 3 types of rooms:"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsx)(n.li,{children:"Shops"}),"\n",(0,r.jsx)(n.li,{children:"Corridors (to navigate from one shop to another)"}),"\n",(0,r.jsx)(n.li,{children:"Large open spaces (contain stairs and escalators to navigate through floors)"}),"\n"]}),"\n",(0,r.jsxs)(n.p,{children:["To keep my example simple, I don't want to have a start and end room. The player will spawn at any shop and the goal is to kill a creature using items found throughout the mall.",(0,r.jsx)(n.br,{}),"\n","This allows me to also demonstrate you are not constrained to a type of dungeon layout."]}),"\n",(0,r.jsx)(n.p,{children:"Here a drawing of a part of the layout we would want to generate (I've colored the rooms depending on their types: blue for corridor, red for shop, green for open space):"}),"\n",(0,r.jsx)(n.p,{children:(0,r.jsx)(n.img,{alt:"Mall-like Layout",src:o(85492).A+"",width:"1174",height:"1063"})}),"\n",(0,r.jsx)(n.admonition,{type:"note",children:(0,r.jsx)(n.p,{children:"I have drawn only one floor of the dungeon to keep the drawing simple, but the open spaces are meant to have doors on multiple floors, so the generated dungeon will have multiple floors like a real mall."})}),"\n",(0,r.jsx)(n.admonition,{type:"tip",children:(0,r.jsxs)(n.p,{children:["I tried to standardize the room sizes to fill the most space possible and create loops easily.",(0,r.jsx)(n.br,{}),"\n","I used the width of a corridor as a room unit to have simpler room size values and to make it scalable easily depending on the camera view chosen when designing the ",(0,r.jsx)(n.a,{href:"/ProceduralDungeon/guides/Best-Practices/Workflows/3Cs",children:"3Cs"}),"."]})}),"\n",(0,r.jsx)(n.h2,{id:"2-deducing-the-pieces-of-the-puzzle",children:"2. Deducing the pieces of the puzzle"}),"\n",(0,r.jsx)(n.p,{children:"Once we've defined the result we want to achieve, we will list all the pieces we need to assemble this puzzle."}),"\n",(0,r.jsx)(n.p,{children:"After drawing the layout I want, I noticed I will need 2 types of doors:"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsx)(n.li,{children:"Doors connecting corridors to shops (in orange). They are automatic sliding doors."}),"\n",(0,r.jsx)(n.li,{children:"Doors connecting corridors between them or with open spaces (in green). They have no meshes at all so this will create a feeling of continuity."}),"\n"]}),"\n",(0,r.jsx)(n.p,{children:"Also, I've used only 4 rooms to get a good looking result:"}),"\n",(0,r.jsx)(n.p,{children:(0,r.jsx)(n.img,{alt:"Mall-like Rooms",src:o(60896).A+"",width:"758",height:"505"})}),"\n",(0,r.jsx)(n.admonition,{type:"note",children:(0,r.jsx)(n.p,{children:"Those rooms are just the room definitions (size and doors), but several rooms can be created for each of them (for example different kind of shops), breaking the feeling of repetitiveness for players while keeping the dungeon layout simple."})}),"\n",(0,r.jsx)(n.h2,{id:"3-deducing-the-constraints",children:"3. Deducing the constraints"}),"\n",(0,r.jsx)(n.p,{children:'Once we have our small pieces and the ideal layout we want to achieve, we can deduce the "constraints" (or "rules") of room placement.'}),"\n",(0,r.jsx)(n.p,{children:"In the case of my example, I noticed those constraints:"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsx)(n.li,{children:"2 open spaces can't be directly connected."}),"\n",(0,r.jsx)(n.li,{children:"2 shops can't be connected directly."}),"\n",(0,r.jsx)(n.li,{children:"There are less corridors 1x1 than corridors 3x1 (about 20% 1x1 and 80% 3x1)"}),"\n",(0,r.jsx)(n.li,{children:"Corridors 1x1 are not directly connected to other corridors 1x1"}),"\n",(0,r.jsx)(n.li,{children:"Open spaces are less likely to to appear than any other corridor (about 10% open space and 90% corridors)"}),"\n"]}),"\n",(0,r.jsx)(n.h2,{id:"4-writing-pseudo-code",children:"4. Writing pseudo-code"}),"\n",(0,r.jsx)(n.p,{children:"Once the constraints have been deduced, we can write quickly a pseudo-code based on those constraints to see how we can determine the next room based on the current room."}),"\n",(0,r.jsxs)(n.p,{children:["I'll use the terms ",(0,r.jsx)(n.code,{children:"currentRoom"})," for the room already generated, and ",(0,r.jsx)(n.code,{children:"door"})," for the door on which the next room will be connected to the ",(0,r.jsx)(n.code,{children:"currentRoom"}),".",(0,r.jsx)(n.br,{}),"\n",'I didn\'t created a "start" and an "end", so I decide to use an open space as first generated room, and stop adding corridors after a number of room is reached (called ',(0,r.jsx)(n.code,{children:"minRoomCount"})," in my pseudo-code)."]}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-txt",metastring:'title="Pseudo-code of ChooseNextRoom function"',children:"if (door.type == shopDoor) and (currentRoom.type != shop) then\r\n    return random shop from a list of shop room\r\nelse if (door.type == corridorDoor) and (rooms.count < minRoomCount) then\r\n    randomNumber := get random number\r\n    if (randomNumber < 10%) then \r\n        return random open space from a list of open space\r\n    else\r\n        randomNumber := get random number\r\n        if (random number < 20%)\r\n            return corridor 1x1\r\n        else\r\n            return corridor 3x1\r\n        endif\r\n    endif\r\nelse\r\n    no room placed\r\nendif\n"})}),"\n",(0,r.jsx)(n.admonition,{type:"note",children:(0,r.jsx)(n.p,{children:"This pseudo-code is a first draft and will need to be iterated on to get the best result. For example the percentage values can be tweaked, the conditions can be modified, new constraints can be added, etc."})}),"\n",(0,r.jsx)(n.p,{children:"Here I'll break down how I've written it:"}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-txt",children:"if (door.type == shopDoor) and (currentRoom.type != shop) then\r\n    return random shop from a list of shop room\n"})}),"\n",(0,r.jsxs)(n.p,{children:["I've placed first the most specific case: when we encounter a shop door, we want to place a shop ",(0,r.jsx)(n.em,{children:"only"})," if the current room is not a shop."]}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-txt",children:"else if (door.type == corridorDoor) and (rooms.count < minRoomCount) then\n"})}),"\n",(0,r.jsx)(n.p,{children:"We want to continue to place corridors at corridor doors until we reach the minimum number of room in the dungeon."}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-txt",children:"    randomNumber := get random number\r\n    if (randomNumber < 10%) then \r\n        return random open space from a list of open space\n"})}),"\n",(0,r.jsxs)(n.p,{children:["We store the random number in a variable, so that this number will not change for each check we do on it.",(0,r.jsx)(n.br,{}),"\n",'Then if this number is below the "open space" threshold, we place an open space room.']}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-txt",children:"    else\r\n        randomNumber := get random number\r\n        if (random number < 20%)\r\n            return corridor 1x1\r\n        else\r\n            return corridor 3x1\r\n        endif\r\n    endif\n"})}),"\n",(0,r.jsx)(n.p,{children:"Here I made an independent random number check because I've deduced a 80-20 ratio of corridors 3x1 and 1x1, but I could've scaled those values down to 90% total to use the same random number as the one used for the open space threshold."}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-txt",children:"else\r\n    no room placed\r\nendif\n"})}),"\n",(0,r.jsx)(n.p,{children:"Finally, if another case than the previous one is encountered, we don't place a room (for example, the dungeon generator may want to place a room from a shop, and in that case we don't want to)."}),"\n",(0,r.jsx)(n.h2,{id:"conclusion",children:"Conclusion"}),"\n",(0,r.jsx)(n.p,{children:"I didn't coded directly the code in blueprint to generate the dungeon.\r\nInstead I wrote on paper the desired result and going through several steps to deduce the code from it."}),"\n",(0,r.jsx)(n.p,{children:"The summary of the steps:"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsx)(n.li,{children:"Defining what are the components (room types) of our dungeon and drawing the desired result."}),"\n",(0,r.jsx)(n.li,{children:"Deducing all the basic pieces (kind of rooms and type of doors)."}),"\n",(0,r.jsx)(n.li,{children:'Deducing the "constraints" (or "rules") between those pieces.'}),"\n",(0,r.jsx)(n.li,{children:"Writing pseudo-code from those constraints."}),"\n"]})]})}function h(e={}){const{wrapper:n}={...(0,t.R)(),...e.components};return n?(0,r.jsx)(n,{...e,children:(0,r.jsx)(c,{...e})}):c(e)}},85492:(e,n,o)=>{o.d(n,{A:()=>r});const r=o.p+"assets/images/mall-like_layout-4600b8d63708082aa5a6f888cede5abd.png"},60896:(e,n,o)=>{o.d(n,{A:()=>r});const r=o.p+"assets/images/mall-like_rooms-b7a33f3ddc4839230d0f3f6d8cddbe26.png"},28453:(e,n,o)=>{o.d(n,{R:()=>i,x:()=>a});var r=o(96540);const t={},s=r.createContext(t);function i(e){const n=r.useContext(s);return r.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function a(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:i(e.components),r.createElement(s.Provider,{value:n},e.children)}}}]);