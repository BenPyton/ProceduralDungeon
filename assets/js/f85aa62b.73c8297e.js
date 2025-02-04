"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[31434],{94739:(e,t,s)=>{s.r(t),s.d(t,{assets:()=>d,contentTitle:()=>r,default:()=>u,frontMatter:()=>i,metadata:()=>a,toc:()=>c});const a=JSON.parse('{"id":"guides/Advanced-Features/Room-Custom-Data","title":"Room Custom Data","description":"You can add variables as much as you want in your RoomData assets. However they are only static variables shared by all room instances spawned with the same RoomData.","source":"@site/versioned_docs/version-3.4/guides/Advanced-Features/Room-Custom-Data.md","sourceDirName":"guides/Advanced-Features","slug":"/guides/Room-Custom-Data","permalink":"/ProceduralDungeon/3.4/guides/Room-Custom-Data","draft":false,"unlisted":false,"tags":[],"version":"3.4","frontMatter":{"slug":"/guides/Room-Custom-Data","pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var o=s(74848),n=s(28453);const i={slug:"/guides/Room-Custom-Data",pagination_prev:null,pagination_next:null},r="Room Custom Data",d={},c=[{value:"Example usage",id:"example-usage",level:2},{value:"Create and using Custom Data",id:"create-and-using-custom-data",level:2}];function l(e){const t={a:"a",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",header:"header",img:"img",p:"p",strong:"strong",...(0,n.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsx)(t.header,{children:(0,o.jsx)(t.h1,{id:"room-custom-data",children:"Room Custom Data"})}),"\n",(0,o.jsxs)(t.p,{children:["You can add variables as much as you want in your ",(0,o.jsx)(t.code,{children:"RoomData"})," assets. However they are only static variables shared by all room instances spawned with the same ",(0,o.jsx)(t.code,{children:"RoomData"}),"."]}),"\n",(0,o.jsxs)(t.p,{children:["Custom data are here to let you add variables and behaviors independents for each of the generated room instances. You ",(0,o.jsxs)(t.strong,{children:["can ",(0,o.jsx)(t.em,{children:"not"})]})," set the value of their variables in room data assets, but you can initialize them during the ",(0,o.jsx)(t.a,{href:"/ProceduralDungeon/3.4/guides/Room-Initialization",children:"Room Initialization"})," phase of the dungeon."]}),"\n",(0,o.jsx)(t.h2,{id:"example-usage",children:"Example usage"}),"\n",(0,o.jsx)(t.p,{children:"Custom data can be useful if you want for example initialize some chest loot in only one room for a boss key."}),"\n",(0,o.jsxs)(t.p,{children:["To achieve that example, you can create a ",(0,o.jsx)(t.code,{children:"BossKeyCustomData"})," class where you define a boolean ",(0,o.jsx)(t.code,{children:"HasBossKey"}),".",(0,o.jsx)(t.br,{}),"\n","This boolean tells whether the room instance should put a boss key in one of its chests."]}),"\n",(0,o.jsxs)(t.p,{children:["Then, in the overridden ",(0,o.jsx)(t.code,{children:"Initialize Dungeon"})," of the dungeon generator, you get all room instances with this custom data and select one of them randomly.",(0,o.jsx)(t.br,{}),"\n","You then access this room's ",(0,o.jsx)(t.code,{children:"BossKeyCustomData"})," and set its ",(0,o.jsx)(t.code,{children:"HasBossKey"})," to true."]}),"\n",(0,o.jsxs)(t.p,{children:["Finally, in the Level Blueprints of rooms with ",(0,o.jsx)(t.code,{children:"BossKeyCustomData"}),", you check if this boolean is true.",(0,o.jsx)(t.br,{}),"\n","And if so you get one if its spawned chest actor to add the boss key in it."]}),"\n",(0,o.jsxs)(t.p,{children:["This example can be done another ways too.\r\nFor example we could instead do something cleaner if we use an inventory system.",(0,o.jsx)(t.br,{}),"\n","We could then have a ",(0,o.jsx)(t.code,{children:"ChestCustomData"})," containing a list of item to put in the room chests.",(0,o.jsx)(t.br,{}),"\n","Then we could just add a boss key item inside that will be used during the level's begin play to put it inside one of the chest actors of the room."]}),"\n",(0,o.jsx)(t.h2,{id:"create-and-using-custom-data",children:"Create and using Custom Data"}),"\n",(0,o.jsxs)(t.p,{children:["To create a custom data, right-click in your content browser and select ",(0,o.jsx)(t.code,{children:"Blueprint Class"}),", then expand the ",(0,o.jsx)(t.code,{children:"All Classes"}),' and type in "Custom Data", then select ',(0,o.jsx)(t.code,{children:"RoomCustomData"})," to create a new child blueprint."]}),"\n",(0,o.jsx)(t.p,{children:(0,o.jsx)(t.img,{src:s(63444).A+"",width:"630",height:"708"})}),"\n",(0,o.jsxs)(t.p,{children:["You can do whatever you want in it like any other blueprint classes.",(0,o.jsx)(t.br,{}),"\n","You can then assign them in the ",(0,o.jsx)(t.code,{children:"Custom Data"})," array of the ",(0,o.jsx)(t.code,{children:"RoomData"})," assets you want."]}),"\n",(0,o.jsx)(t.p,{children:(0,o.jsx)(t.img,{src:s(80091).A+"",width:"603",height:"245"})}),"\n",(0,o.jsxs)(t.p,{children:["As said above, you can't set variables of the custom data directly from the data asset.",(0,o.jsx)(t.br,{}),"\n","However, you can get a reference of the instantiated custom data from anywhere you have a access to a room instance, with the ",(0,o.jsx)(t.code,{children:"Get Custom Data"})," function of the room instance.\r\nFor example you can access from the ",(0,o.jsx)(t.code,{children:"Room"})," getter in the room level blueprint."]}),"\n",(0,o.jsx)(t.p,{children:(0,o.jsx)(t.img,{src:s(85286).A+"",width:"874",height:"216"})})]})}function u(e={}){const{wrapper:t}={...(0,n.R)(),...e.components};return t?(0,o.jsx)(t,{...e,children:(0,o.jsx)(l,{...e})}):l(e)}},80091:(e,t,s)=>{s.d(t,{A:()=>a});const a=s.p+"assets/images/AssignCustomData-a449e294c5e0ec8b15280fc995eb22fd.gif"},63444:(e,t,s)=>{s.d(t,{A:()=>a});const a=s.p+"assets/images/CreateCustomData-a57f6ed006fd595c3eb3bddf36c329f4.gif"},85286:(e,t,s)=>{s.d(t,{A:()=>a});const a=s.p+"assets/images/CustomDataUsage-41133f6f93909fe1a517b3e28fdd813c.jpg"},28453:(e,t,s)=>{s.d(t,{R:()=>i,x:()=>r});var a=s(96540);const o={},n=a.createContext(o);function i(e){const t=a.useContext(n);return a.useMemo((function(){return"function"==typeof e?e(t):{...t,...e}}),[t,e])}function r(e){let t;return t=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:i(e.components),a.createElement(n.Provider,{value:t},e.children)}}}]);