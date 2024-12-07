"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[2086],{4066:(e,o,r)=>{r.r(o),r.d(o,{assets:()=>c,contentTitle:()=>d,default:()=>h,frontMatter:()=>s,metadata:()=>i,toc:()=>l});var n=r(74848),t=r(28453);const s={pagination_prev:null,pagination_next:null},d="Room",i={id:"api/Classes/Room/Room",title:"Room",description:"Procedural Dungeon \u23f5",source:"@site/docs/api/Classes/Room/Room.md",sourceDirName:"api/Classes/Room",slug:"/api/Classes/Room/",permalink:"/ProceduralDungeon/api/Classes/Room/",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},c={},l=[{value:"Class Details",id:"class-details",level:2},{value:"Description",id:"description",level:2},{value:"Nodes",id:"nodes",level:2}];function a(e){const o={a:"a",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",header:"header",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,t.R)(),...e.components};return(0,n.jsxs)(n.Fragment,{children:[(0,n.jsxs)(o.p,{children:[(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\r\nRoom"]}),"\n",(0,n.jsx)(o.header,{children:(0,n.jsx)(o.h1,{id:"room",children:"Room"})}),"\n",(0,n.jsx)(o.h2,{id:"class-details",children:"Class Details"}),"\n",(0,n.jsxs)(o.p,{children:[(0,n.jsx)(o.strong,{children:"Defined in:"})," ",(0,n.jsx)(o.code,{children:"Public/Room.h"}),(0,n.jsx)(o.br,{}),"\n",(0,n.jsx)(o.strong,{children:"Hierarchy:"})," ",(0,n.jsx)(o.em,{children:"Object \u2192 Replicable Object \u2192 Room"}),(0,n.jsx)(o.br,{}),"\n",(0,n.jsx)(o.strong,{children:"Implements:"})," ",(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Classes/ReadOnlyRoom/",children:"Read Only Room"}),(0,n.jsx)(o.br,{}),"\n",(0,n.jsx)(o.strong,{children:"Exposed in blueprint as:"})," Variable\xa0Type"]}),"\n",(0,n.jsx)(o.h2,{id:"description",children:"Description"}),"\n",(0,n.jsxs)(o.p,{children:["The room instances of the dungeon.",(0,n.jsx)("br",{}),"Holds data specific to each room instance, e.g. location, direction, is player inside, room custom data, etc."]}),"\n",(0,n.jsx)(o.h2,{id:"nodes",children:"Nodes"}),"\n",(0,n.jsxs)(o.table,{children:[(0,n.jsx)(o.thead,{children:(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.th,{children:"Name"}),(0,n.jsx)(o.th,{children:"Category"}),(0,n.jsx)(o.th,{children:"Description"})]})}),(0,n.jsxs)(o.tbody,{children:[(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/ForceVisibility",children:"Force Visibility"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Force the room to be veisible"})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetAllConnectedRooms",children:"Get All Connected Rooms"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Returns all the room instances connected with this one."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetAllDoors",children:"Get All Doors"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Fill an array with all the door actors connected to the room."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetConnectedRoomAt",children:"Get Connected Room At"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Returns the connected room instance at DoorIndex."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetConnectedRoomIndex",children:"Get Connected Room Index"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Returns the index of the provided room, or -1 if room is not connected."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetCustomData_BP",children:"Get Custom Data"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Access to custom data of the room."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetDoor",children:"Get Door"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Get the door actor from a specific index."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetDoorsWith",children:"Get Doors With"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsxs)(o.td,{children:["Returns the door actor shared with the provided room.",(0,n.jsx)("br",{}),"Returns null if the provided room is not connected with this."]})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/GetRandomStream",children:"Get Random Stream"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Returns the RandomStream from the Dungeon Generator"})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/HasCustomData_BP",children:"Has Custom Data"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Check if the room instance contains a custom data of a specific type."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/IsDoorConnected",children:"Is Door Connected"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Returns true if the door at DoorIndex is connected to another room."})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/IsLocked",children:"Is Locked"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsxs)(o.td,{children:["Is the room locked?",(0,n.jsx)("br",{}),"If it is, the doors will be locked (except if they have ",(0,n.jsx)(o.code,{children:"Alway Unlocked"}),")."]})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/IsPlayerInside",children:"Is Player Inside"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsxs)(o.td,{children:["Is the player currently inside the room?",(0,n.jsx)("br",{}),"A player can be in multiple rooms at once, for example when he stands at the door frame,",(0,n.jsx)("br",{}),"the player's capsule is in both rooms."]})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/IsVisible",children:"Is Visible"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsx)(o.td,{children:"Is the room currently visible?"})]}),(0,n.jsxs)(o.tr,{children:[(0,n.jsx)(o.td,{children:(0,n.jsx)(o.a,{href:"/ProceduralDungeon/api/Room/Lock",children:"Lock"})}),(0,n.jsx)(o.td,{children:"Room"}),(0,n.jsxs)(o.td,{children:["Lock or unlock the room instance.",(0,n.jsx)("br",{}),"Will lock/unlock the doors too (except if they have ",(0,n.jsx)(o.code,{children:"Alway Unlocked"}),")."]})]})]})]})]})}function h(e={}){const{wrapper:o}={...(0,t.R)(),...e.components};return o?(0,n.jsx)(o,{...e,children:(0,n.jsx)(a,{...e})}):a(e)}},28453:(e,o,r)=>{r.d(o,{R:()=>d,x:()=>i});var n=r(96540);const t={},s=n.createContext(t);function d(e){const o=n.useContext(s);return n.useMemo((function(){return"function"==typeof e?e(o):{...o,...e}}),[o,e])}function i(e){let o;return o=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:d(e.components),n.createElement(s.Provider,{value:o},e.children)}}}]);