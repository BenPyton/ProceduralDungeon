"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[930],{37387:(e,n,i)=>{i.r(n),i.d(n,{assets:()=>d,contentTitle:()=>s,default:()=>h,frontMatter:()=>a,metadata:()=>r,toc:()=>c});var o=i(74848),t=i(28453);const a={pagination_prev:null,pagination_next:null},s="Room Initialization",r={id:"guides/Room-Initialization",title:"Room Initialization",description:"Before validating a dungeon, you can do some initialization to the room instances generated by the dungeon.",source:"@site/versioned_docs/version-3.3/guides/Room-Initialization.md",sourceDirName:"guides",slug:"/guides/Room-Initialization",permalink:"/ProceduralDungeon/3.3/guides/Room-Initialization",draft:!1,unlisted:!1,tags:[],version:"3.3",frontMatter:{pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},d={},c=[{value:"Inside the Room Data",id:"inside-the-room-data",level:2},{value:"Inside the Dungeon Generator",id:"inside-the-dungeon-generator",level:2}];function l(e){const n={a:"a",br:"br",code:"code",h1:"h1",h2:"h2",header:"header",img:"img",p:"p",strong:"strong",...(0,t.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsx)(n.header,{children:(0,o.jsx)(n.h1,{id:"room-initialization",children:"Room Initialization"})}),"\n",(0,o.jsx)(n.p,{children:"Before validating a dungeon, you can do some initialization to the room instances generated by the dungeon."}),"\n",(0,o.jsxs)(n.p,{children:["Room instances are ",(0,o.jsx)(n.strong,{children:"NOT"})," room levels, so you do not have access to actors inside the levels.",(0,o.jsx)(n.br,{}),"\n","If you want to initialize actors, you have to put your code in the ",(0,o.jsx)(n.code,{children:"BeginPlay"})," of your blueprint levels where you can access their room instance to initialize the actors."]}),"\n",(0,o.jsxs)(n.p,{children:["Room instances do not have many thing to initialize by default, but you can extend their instance specific data by using ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Room-Custom-Data",children:"Room Custom Data"}),"."]}),"\n",(0,o.jsx)(n.h2,{id:"inside-the-room-data",children:"Inside the Room Data"}),"\n",(0,o.jsxs)(n.p,{children:["The first place where you can initialize instances values is inside your ",(0,o.jsx)(n.code,{children:"RoomData"})," ",(0,o.jsx)(n.strong,{children:"child blueprints"}),".\r\nYou can override the function ",(0,o.jsx)(n.code,{children:"Initialize Room"})," which is called just after the last ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Continue-To-Add-Room",children:"Continue To Add Room"})," (the one returning false) and before ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Is-Valid-Dungeon",children:"Is Valid Dungeon"}),"."]}),"\n",(0,o.jsx)(n.p,{children:(0,o.jsx)(n.img,{src:i(48743).A+"",width:"396",height:"271"})}),"\n",(0,o.jsxs)(n.p,{children:["Inside it we can access the room instance's functions (e.g. ",(0,o.jsx)(n.code,{children:"Lock"}),") and its custom data.",(0,o.jsx)(n.br,{}),"\n","For the examples, in the following pictures I will use a ",(0,o.jsx)(n.code,{children:"Secret Room"})," and a ",(0,o.jsx)(n.code,{children:"Chest"})," custom data but they are not provided by the plugin."]}),"\n",(0,o.jsxs)(n.p,{children:["A simple example is to lock the room if it has a ",(0,o.jsx)(n.code,{children:"Secret Room"})," custom data."]}),"\n",(0,o.jsx)(n.p,{children:(0,o.jsx)(n.img,{src:i(57604).A+"",width:"783",height:"271"})}),"\n",(0,o.jsx)(n.p,{children:"A little more complex example is to also put a key in another room (e.g. a room with a chest) so the player will be able to unlock the secret room."}),"\n",(0,o.jsx)(n.p,{children:(0,o.jsx)(n.img,{src:i(84468).A+"",width:"1636",height:"848"})}),"\n",(0,o.jsx)(n.h2,{id:"inside-the-dungeon-generator",children:"Inside the Dungeon Generator"}),"\n",(0,o.jsxs)(n.p,{children:["The second place where you can initialize room instances is the ",(0,o.jsx)(n.code,{children:"Initialize Dungeon"})," function of the ",(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Dungeon-Generator",children:"Dungeon Generator"}),"."]}),"\n",(0,o.jsxs)(n.p,{children:["You can do the same thing here than in the ",(0,o.jsx)(n.code,{children:"Initialize Room"})," above. You'll just have to get the room you want first.",(0,o.jsx)(n.br,{}),"\n","The example below shows a way to initialize just one ",(0,o.jsx)(n.code,{children:"Secret Room"})," in the dungeon:"]}),"\n",(0,o.jsx)(n.p,{children:(0,o.jsx)(n.img,{src:i(77610).A+"",width:"1327",height:"285"})})]})}function h(e={}){const{wrapper:n}={...(0,t.R)(),...e.components};return n?(0,o.jsx)(n,{...e,children:(0,o.jsx)(l,{...e})}):l(e)}},57604:(e,n,i)=>{i.d(n,{A:()=>o});const o=i.p+"assets/images/DataInitializeRoom-499e5dc706e389b380e3f055653f1899.jpg"},84468:(e,n,i)=>{i.d(n,{A:()=>o});const o=i.p+"assets/images/DataInitializeRoom2-830abdfd1b56238fd71c400fa76aa33c.jpg"},77610:(e,n,i)=>{i.d(n,{A:()=>o});const o=i.p+"assets/images/DungeonInitializeRoom-fea1684493c736d25fcf963b218dba85.jpg"},48743:(e,n,i)=>{i.d(n,{A:()=>o});const o=i.p+"assets/images/OverrideInitializeRoom-dfe65648da8381869ebccca02a5aa2e5.gif"},28453:(e,n,i)=>{i.d(n,{R:()=>s,x:()=>r});var o=i(96540);const t={},a=o.createContext(t);function s(e){const n=o.useContext(a);return o.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function r(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:s(e.components),o.createElement(a.Provider,{value:n},e.children)}}}]);