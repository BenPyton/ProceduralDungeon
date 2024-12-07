"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[6467],{91346:(e,n,r)=>{r.r(n),r.d(n,{assets:()=>a,contentTitle:()=>d,default:()=>c,frontMatter:()=>s,metadata:()=>i,toc:()=>l});var t=r(74848),o=r(28453);const s={pagination_prev:null,pagination_next:null},d="Dungeon Generator",i={id:"api/Classes/DungeonGenerator/DungeonGenerator",title:"DungeonGenerator",description:"Procedural Dungeon \u23f5",source:"@site/docs/api/Classes/DungeonGenerator/DungeonGenerator.md",sourceDirName:"api/Classes/DungeonGenerator",slug:"/api/Classes/DungeonGenerator/",permalink:"/ProceduralDungeon/api/Classes/DungeonGenerator/",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},a={},l=[{value:"Class Details",id:"class-details",level:2},{value:"Description",id:"description",level:2},{value:"Events",id:"events",level:2},{value:"Properties",id:"properties",level:2},{value:"Nodes",id:"nodes",level:2}];function h(e){const n={a:"a",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",header:"header",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,o.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsxs)(n.p,{children:[(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\r\nDungeon Generator"]}),"\n",(0,t.jsx)(n.header,{children:(0,t.jsx)(n.h1,{id:"dungeon-generator",children:"Dungeon Generator"})}),"\n",(0,t.jsx)(n.h2,{id:"class-details",children:"Class Details"}),"\n",(0,t.jsxs)(n.p,{children:[(0,t.jsx)(n.strong,{children:"Defined in:"})," ",(0,t.jsx)(n.code,{children:"Public/DungeonGenerator.h"}),(0,t.jsx)(n.br,{}),"\n",(0,t.jsx)(n.strong,{children:"Hierarchy:"})," ",(0,t.jsx)(n.em,{children:"Object \u2192 Actor \u2192 Dungeon Generator Base \u2192 Dungeon Generator"}),(0,t.jsx)(n.br,{}),"\n",(0,t.jsx)(n.strong,{children:"Exposed in blueprint as:"})," Blueprint\xa0Base\xa0Class | Variable\xa0Type"]}),"\n",(0,t.jsx)(n.h2,{id:"description",children:"Description"}),"\n",(0,t.jsx)(n.p,{children:"This is the main actor of the plugin. The dungeon generator is responsible to generate dungeons and replicate them over the network."}),"\n",(0,t.jsx)(n.h2,{id:"events",children:"Events"}),"\n",(0,t.jsxs)(n.table,{children:[(0,t.jsx)(n.thead,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.th,{children:"Name"}),(0,t.jsx)(n.th,{children:"Category"}),(0,t.jsx)(n.th,{children:"Description"})]})}),(0,t.jsxs)(n.tbody,{children:[(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["On\xa0Failed\xa0to\xa0Add\xa0Room\xa0Event",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsx)(n.td,{children:"Called each time no room could have been placed at a door (all room placement tries have been exhausted)."})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["On\xa0Generation\xa0Failed\xa0Event",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsxs)(n.td,{children:["Called when all dungeon generation tries are exhausted (IsValidDungeon always return false).",(0,t.jsx)("br",{}),"No dungeon had been generated."]})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["On\xa0Generation\xa0Init\xa0Event",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsx)(n.td,{children:"Called before trying to generate a new dungeon and each time IsValidDungeon return false."})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["On\xa0Post\xa0Generation\xa0Event",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsx)(n.td,{children:"Called once after all the dungeon generation (even if failed)."})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["On\xa0Pre\xa0Generation\xa0Event",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsx)(n.td,{children:"Called once before anything else when generating a new dungeon."})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["On\xa0Room\xa0Added\xa0Event",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsxs)(n.td,{children:["Called each time a room is added in the dungeon (but not spawned yet).",(0,t.jsx)("br",{}),"Those rooms can be destroyed without loading them if the generation try is not valid."]})]})]})]}),"\n",(0,t.jsx)(n.h2,{id:"properties",children:"Properties"}),"\n",(0,t.jsxs)(n.table,{children:[(0,t.jsx)(n.thead,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.th,{children:"Name"}),(0,t.jsx)(n.th,{children:"Type"}),(0,t.jsx)(n.th,{children:"Category"}),(0,t.jsx)(n.th,{children:"Accessors"}),(0,t.jsx)(n.th,{children:"Description"})]})}),(0,t.jsxs)(n.tbody,{children:[(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/DungeonGenerator/bAutoDiscardRoomIfNull",children:"Auto Discard Room if Null"})}),(0,t.jsx)(n.td,{children:"bool"}),(0,t.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsx)(n.td,{children:"If true, returning null in ChooseNextRoom is the same as calling DiscardRoom."})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/DungeonGenerator/bCanLoop",children:"Can Loop"})}),(0,t.jsx)(n.td,{children:"bool"}),(0,t.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsxs)(n.td,{children:["If ticked, newly placed room will check if any door is aligned with another room, and if so will connect them.",(0,t.jsx)("br",{}),"If unticked, only the doors between CurrentRoom and NextRoom (in the function ChooseNextRoom) will be connected.",(0,t.jsx)("br",{}),"(will only have effect if the deprecated CanLoop in the plugin settings is ticked too, until it is removed in a future version)"]})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/DungeonGenerator/DungeonLimits",children:"Dungeon Limits"})}),(0,t.jsx)(n.td,{children:"FBoundsParams"}),(0,t.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsx)(n.td,{})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/DungeonGenerator/GenerationType",children:"Generation Type"})}),(0,t.jsx)(n.td,{children:"EGenerationType"}),(0,t.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsxs)(n.td,{children:["In which order the dungeon generate rooms.",(0,t.jsx)("br",{}),"Depth First: Dungeon will use the last generated room to place the next one. Resulting in a more linear dungeon.",(0,t.jsx)("br",{}),"Breadth First: Dungeon will generate a room at each door of the current one before going to the next room. Resulting in a more spread dungeon."]})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["Rooms",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"UDungeonGraph*"}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsx)(n.td,{children:"Blueprint\xa0Read\xa0Only"}),(0,t.jsx)(n.td,{})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["Use\xa0Generator\xa0Transform",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"bool"}),(0,t.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsxs)(n.td,{children:["If ticked, the rooms location and rotation will be relative to this actor transform.",(0,t.jsx)("br",{}),"If unticked, the rooms will be placed relatively to the world's origin."]})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["Use\xa0World\xa0Collision\xa0Checks",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"bool"}),(0,t.jsx)(n.td,{children:"Procedural\xa0Generation"}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsxs)(n.td,{children:["If ticked, when trying to place a new room during a dungeon generation,",(0,t.jsx)("br",{}),"a box overlap test will be made to make sure the room will not spawn",(0,t.jsx)("br",{}),"inside existing meshes in the persistent world.",(0,t.jsx)("br",{}),"This is a heavy work and should be ticked only when necessary.",(0,t.jsx)("br",{}),"Does not have impact during gameplay. Only during the generation process."]})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["Seed",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"uint32"}),(0,t.jsxs)(n.td,{children:["Procedural\xa0Generation\xa0",(0,t.jsx)("br",{}),"\u2514\xa0Seed"]}),(0,t.jsx)(n.td,{children:"Edit\xa0Anywhere"}),(0,t.jsx)(n.td,{})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["Seed\xa0Increment",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"uint32"}),(0,t.jsxs)(n.td,{children:["Procedural\xa0Generation\xa0",(0,t.jsx)("br",{}),"\u2514\xa0Seed"]}),(0,t.jsx)(n.td,{children:"Edit\xa0Anywhere"}),(0,t.jsx)(n.td,{children:"The increment number for each subsequent dungeon generation when SeedType is AutoIncrement."})]}),(0,t.jsxs)(n.tr,{children:[(0,t.jsxs)(n.td,{children:["Seed\xa0Type",(0,t.jsx)("br",{}),"(inherited from Dungeon\xa0Generator\xa0Base)"]}),(0,t.jsx)(n.td,{children:"ESeedType"}),(0,t.jsxs)(n.td,{children:["Procedural\xa0Generation\xa0",(0,t.jsx)("br",{}),"\u2514\xa0Seed"]}),(0,t.jsxs)(n.td,{children:["Blueprint\xa0Read/Write",(0,t.jsx)("br",{}),"Edit\xa0Anywhere"]}),(0,t.jsxs)(n.td,{children:["How to handle the seed at each generation call.",(0,t.jsx)("br",{}),"Random: Generate and use a random seed.",(0,t.jsx)("br",{}),"Auto Increment: Use Seed for first generation, and increment it by SeedIncrement in each subsequent generation.",(0,t.jsx)("br",{}),"Fixed: Use only Seed for each generation."]})]})]})]}),"\n",(0,t.jsx)(n.h2,{id:"nodes",children:"Nodes"}),"\n",(0,t.jsxs)(n.table,{children:[(0,t.jsx)(n.thead,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.th,{children:"Name"}),(0,t.jsx)(n.th,{children:"Category"}),(0,t.jsx)(n.th,{children:"Description"})]})}),(0,t.jsx)(n.tbody,{children:(0,t.jsxs)(n.tr,{children:[(0,t.jsx)(n.td,{children:(0,t.jsx)(n.a,{href:"/ProceduralDungeon/api/DungeonGenerator/DiscardRoom",children:"Discard Room"})}),(0,t.jsx)(n.td,{children:"Dungeon\xa0Generator"}),(0,t.jsxs)(n.td,{children:['Must be called in "Choose Next Room" function to be used.',(0,t.jsx)("br",{}),"Tell explicitely the generator we don't want to place a room for a specific door.",(0,t.jsx)("br",{}),"No error will be thrown when returning a null room data and no further room placement tries occur for this door (skip directly to the next door)."]})]})})]})]})}function c(e={}){const{wrapper:n}={...(0,o.R)(),...e.components};return n?(0,t.jsx)(n,{...e,children:(0,t.jsx)(h,{...e})}):h(e)}},28453:(e,n,r)=>{r.d(n,{R:()=>d,x:()=>i});var t=r(96540);const o={},s=t.createContext(o);function d(e){const n=t.useContext(s);return t.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function i(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:d(e.components),t.createElement(s.Provider,{value:n},e.children)}}}]);