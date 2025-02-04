"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[98651],{85076:(e,n,r)=>{r.r(n),r.d(n,{assets:()=>a,contentTitle:()=>i,default:()=>c,frontMatter:()=>s,metadata:()=>t,toc:()=>l});const t=JSON.parse('{"id":"api/Classes/DungeonGenerator/DungeonGenerator","title":"DungeonGenerator","description":"Procedural Dungeon \u23f5","source":"@site/versioned_docs/version-3.3/api/Classes/DungeonGenerator/DungeonGenerator.md","sourceDirName":"api/Classes/DungeonGenerator","slug":"/api/Classes/DungeonGenerator/","permalink":"/ProceduralDungeon/3.3/api/Classes/DungeonGenerator/","draft":false,"unlisted":false,"tags":[],"version":"3.3","frontMatter":{"pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var o=r(74848),d=r(28453);const s={pagination_prev:null,pagination_next:null},i="Dungeon Generator",a={},l=[{value:"Class Details",id:"class-details",level:2},{value:"Description",id:"description",level:2},{value:"Properties",id:"properties",level:2},{value:"Nodes",id:"nodes",level:2}];function h(e){const n={a:"a",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",header:"header",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,d.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsxs)(n.p,{children:[(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/",children:"Procedural Dungeon"})," \u23f5\r\nDungeon Generator"]}),"\n",(0,o.jsx)(n.header,{children:(0,o.jsx)(n.h1,{id:"dungeon-generator",children:"Dungeon Generator"})}),"\n",(0,o.jsx)(n.h2,{id:"class-details",children:"Class Details"}),"\n",(0,o.jsxs)(n.p,{children:[(0,o.jsx)(n.strong,{children:"Defined in:"})," ",(0,o.jsx)(n.code,{children:"Public/DungeonGenerator.h"}),(0,o.jsx)(n.br,{}),"\n",(0,o.jsx)(n.strong,{children:"Hierarchy:"})," ",(0,o.jsx)(n.em,{children:"Object \u2192 Actor \u2192 Dungeon Generator"}),(0,o.jsx)(n.br,{}),"\n",(0,o.jsx)(n.strong,{children:"Exposed in blueprint as:"})," Blueprint\xa0Base\xa0Class | Variable\xa0Type"]}),"\n",(0,o.jsx)(n.h2,{id:"description",children:"Description"}),"\n",(0,o.jsx)(n.p,{children:"This is the main actor of the plugin. The dungeon generator is responsible to generate dungeons and replicate them over the network."}),"\n",(0,o.jsx)(n.h2,{id:"properties",children:"Properties"}),"\n",(0,o.jsxs)(n.table,{children:[(0,o.jsx)(n.thead,{children:(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.th,{children:"Name"}),(0,o.jsx)(n.th,{children:"Type"}),(0,o.jsx)(n.th,{children:"Category"}),(0,o.jsx)(n.th,{children:"Accessors"}),(0,o.jsx)(n.th,{children:"Description"})]})}),(0,o.jsxs)(n.tbody,{children:[(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Rooms"}),(0,o.jsx)(n.td,{children:"UDungeonGraph*"}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{children:"Blueprint\xa0Read\xa0Only"}),(0,o.jsx)(n.td,{})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Can\xa0Loop"}),(0,o.jsx)(n.td,{children:"bool"}),(0,o.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,o.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsxs)(n.td,{children:["If ticked, newly placed room will check if any door is aligned with another room, and if so will connect them.",(0,o.jsx)("br",{}),"If unticked, only the doors between CurrentRoom and NextRoom (in the function ChooseNextRoom) will be connected.",(0,o.jsx)("br",{}),"(will only have effect if the deprecated CanLoop in the plugin settings is ticked too, until it is removed in a future version)"]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Dungeon\xa0Limits"}),(0,o.jsx)(n.td,{children:"FBoundsParams"}),(0,o.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,o.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsx)(n.td,{})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Generation\xa0Type"}),(0,o.jsx)(n.td,{children:"EGenerationType"}),(0,o.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,o.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsxs)(n.td,{children:["In which order the dungeon generate rooms.",(0,o.jsx)("br",{}),"Depth First: Dungeon will use the last generated room to place the next one. Resulting in a more linear dungeon.",(0,o.jsx)("br",{}),"Breadth First: Dungeon will generate a room at each door of the current one before going to the next room. Resulting in a more spread dungeon."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Use\xa0Generator\xa0Transform"}),(0,o.jsx)(n.td,{children:"bool"}),(0,o.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,o.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsxs)(n.td,{children:["If ticked, the rooms location and rotation will be relative to this actor transform.",(0,o.jsx)("br",{}),"If unticked, the rooms will be placed relatively to the world's origin."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Use\xa0World\xa0Collision\xa0Checks"}),(0,o.jsx)(n.td,{children:"bool"}),(0,o.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,o.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsxs)(n.td,{children:["If ticked, when trying to place a new room during a dungeon generation,",(0,o.jsx)("br",{}),"a box overlap test will be made to make sure the room will not spawn",(0,o.jsx)("br",{}),"inside existing meshes in the persistent world.",(0,o.jsx)("br",{}),"This is a heavy work and should be ticked only when necessary.",(0,o.jsx)("br",{}),"Does not have impact during gameplay. Only during the generation process."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Seed"}),(0,o.jsx)(n.td,{children:"uint32"}),(0,o.jsxs)(n.td,{children:["Procedural\xa0Generation\xa0",(0,o.jsx)("br",{}),"\u2514\xa0Seed"]}),(0,o.jsx)(n.td,{children:"Edit\xa0Anywhere"}),(0,o.jsx)(n.td,{})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Seed\xa0Increment"}),(0,o.jsx)(n.td,{children:"uint32"}),(0,o.jsxs)(n.td,{children:["Procedural\xa0Generation\xa0",(0,o.jsx)("br",{}),"\u2514\xa0Seed"]}),(0,o.jsx)(n.td,{children:"Edit\xa0Anywhere"}),(0,o.jsx)(n.td,{children:"The increment number for each subsequent dungeon generation when SeedType is AutoIncrement."})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:"Seed\xa0Type"}),(0,o.jsx)(n.td,{children:"ESeedType"}),(0,o.jsxs)(n.td,{children:["Procedural\xa0Generation\xa0",(0,o.jsx)("br",{}),"\u2514\xa0Seed"]}),(0,o.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,o.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,o.jsxs)(n.td,{children:["How to handle the seed at each generation call.",(0,o.jsx)("br",{}),"Random: Generate and use a random seed.",(0,o.jsx)("br",{}),"Auto Increment: Use Seed for first generation, and increment it by SeedIncrement in each subsequent generation.",(0,o.jsx)("br",{}),"Fixed: Use only Seed for each generation."]})]})]})]}),"\n",(0,o.jsx)(n.h2,{id:"nodes",children:"Nodes"}),"\n",(0,o.jsxs)(n.table,{children:[(0,o.jsx)(n.thead,{children:(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.th,{children:"Name"}),(0,o.jsx)(n.th,{children:"Category"}),(0,o.jsx)(n.th,{children:"Description"})]})}),(0,o.jsxs)(n.tbody,{children:[(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/Generate",children:"Generate"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsxs)(n.td,{children:["Update the seed and call the generation on all clients",(0,o.jsx)("br",{}),"Do nothing when called on clients"]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/GetCompatibleRoomData",children:"Get Compatible Room Data"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{children:"Returns an array of room data with compatible at least one compatible door with the door data provided."})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/GetProgress",children:"Get Progress"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{children:"Returns the current generation progress."})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/GetRandomRoomData",children:"Get Random Room Data"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{children:"Return a random RoomData from the array provided"})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/GetRandomRoomDataWeighted",children:"Get Random Room Data Weighted"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsxs)(n.td,{children:["Return a random RoomData from the weighted map provided.",(0,o.jsx)("br",{}),"For example: you have RoomA with weight 1 and RoomB with weight 2,",(0,o.jsx)("br",{}),"then RoomA has proba of 1/3 and RoomB 2/3 to be returned."]})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/GetRandomStream",children:"Get Random Stream"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{children:"Access to the random stream of the procedural dungeon. You should always use this for the procedural generation."})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/GetSeed",children:"Get Seed"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/SetSeed",children:"Set Seed"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsx)(n.td,{})]}),(0,o.jsxs)(n.tr,{children:[(0,o.jsx)(n.td,{children:(0,o.jsx)(n.a,{href:"/ProceduralDungeon/3.3/api/DungeonGenerator/Unload",children:"Unload"})}),(0,o.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,o.jsxs)(n.td,{children:["Unload the current dungeon",(0,o.jsx)("br",{}),"Do nothing when called on clients"]})]})]})]})]})}function c(e={}){const{wrapper:n}={...(0,d.R)(),...e.components};return n?(0,o.jsx)(n,{...e,children:(0,o.jsx)(h,{...e})}):h(e)}},28453:(e,n,r)=>{r.d(n,{R:()=>s,x:()=>i});var t=r(96540);const o={},d=t.createContext(o);function s(e){const n=t.useContext(d);return t.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function i(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:s(e.components),t.createElement(d.Provider,{value:n},e.children)}}}]);