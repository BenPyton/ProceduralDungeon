"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[1405],{72143:(n,e,o)=>{o.r(e),o.d(e,{assets:()=>i,contentTitle:()=>t,default:()=>l,frontMatter:()=>s,metadata:()=>c,toc:()=>h});var r=o(74848),a=o(28453);const s={slug:"/api/RoomData/RandomDoor",pagination_prev:null,pagination_next:null},t="Random Door",c={id:"api/Classes/RoomData/Variables/RandomDoor/RandomDoor",title:"RandomDoor",description:"Procedural Dungeon \u23f5",source:"@site/docs/api/Classes/RoomData/Variables/RandomDoor/RandomDoor.md",sourceDirName:"api/Classes/RoomData/Variables/RandomDoor",slug:"/api/RoomData/RandomDoor",permalink:"/ProceduralDungeon/api/RoomData/RandomDoor",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/api/RoomData/RandomDoor",pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},i={},h=[{value:"Description",id:"description",level:2},{value:"Nodes",id:"nodes",level:4}];function d(n){const e={a:"a",br:"br",h1:"h1",h2:"h2",h4:"h4",header:"header",img:"img",p:"p",strong:"strong",...(0,a.R)(),...n.components};return(0,r.jsxs)(r.Fragment,{children:[(0,r.jsxs)(e.p,{children:[(0,r.jsx)(e.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\r\n",(0,r.jsx)(e.a,{href:"/ProceduralDungeon/api/Classes/RoomData/",children:"Room Data"})," \u23f5\r\nRandom Door"]}),"\n",(0,r.jsx)(e.header,{children:(0,r.jsx)(e.h1,{id:"random-door",children:"Random Door"})}),"\n",(0,r.jsxs)(e.p,{children:[(0,r.jsx)(e.strong,{children:"Class:"})," ",(0,r.jsx)(e.a,{href:"/ProceduralDungeon/api/Classes/RoomData/",children:"Room\xa0Data"}),(0,r.jsx)(e.br,{}),"\n",(0,r.jsx)(e.strong,{children:"Category:"})," Doors",(0,r.jsx)(e.br,{}),"\n",(0,r.jsx)(e.strong,{children:"Type:"})," Boolean",(0,r.jsx)(e.br,{}),"\n",(0,r.jsx)(e.strong,{children:"Editor Access:"})," Anywhere",(0,r.jsx)(e.br,{}),"\n",(0,r.jsx)(e.strong,{children:"Blueprint Access:"})," Read\xa0Only"]}),"\n",(0,r.jsx)(e.h2,{id:"description",children:"Description"}),"\n",(0,r.jsxs)(e.p,{children:["This will force a random door to be chosen during the dungeon generation.",(0,r.jsx)("br",{}),"DEPRECATED: It will be removed in a future version of the plugin. As a replacement, you should return -1 as DoorIndex in the ChooseNextRoomData of your DungeonGenerator."]}),"\n",(0,r.jsx)(e.h4,{id:"nodes",children:"Nodes"}),"\n",(0,r.jsx)(e.p,{children:(0,r.jsx)(e.img,{src:o(72627).A+"",width:"219",height:"38"})})]})}function l(n={}){const{wrapper:e}={...(0,a.R)(),...n.components};return e?(0,r.jsx)(e,{...n,children:(0,r.jsx)(d,{...n})}):d(n)}},72627:(n,e,o)=>{o.d(e,{A:()=>r});const r="data:image/png;base64,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"},28453:(n,e,o)=>{o.d(e,{R:()=>t,x:()=>c});var r=o(96540);const a={},s=r.createContext(a);function t(n){const e=r.useContext(s);return r.useMemo((function(){return"function"==typeof n?n(e):{...e,...n}}),[e,n])}function c(n){let e;return e=n.disableParentContext?"function"==typeof n.components?n.components(a):n.components||a:t(n.components),r.createElement(s.Provider,{value:e},n.children)}}}]);