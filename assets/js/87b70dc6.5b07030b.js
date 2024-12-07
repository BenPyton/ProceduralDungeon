"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[5706],{21535:(e,n,o)=>{o.r(n),o.d(n,{assets:()=>a,contentTitle:()=>i,default:()=>c,frontMatter:()=>s,metadata:()=>d,toc:()=>l});var r=o(74848),t=o(28453);const s={pagination_prev:"guides/Creating-Rooms",pagination_next:"guides/Choose-First-Room-Data"},i="Dungeon Generator",d={id:"guides/Dungeon-Generator",title:"Dungeon Generator",description:"Create a dungeon generator",source:"@site/versioned_docs/version-3.3/guides/Dungeon-Generator.md",sourceDirName:"guides",slug:"/guides/Dungeon-Generator",permalink:"/ProceduralDungeon/3.3/guides/Dungeon-Generator",draft:!1,unlisted:!1,tags:[],version:"3.3",frontMatter:{pagination_prev:"guides/Creating-Rooms",pagination_next:"guides/Choose-First-Room-Data"},sidebar:"homeSidebar",previous:{title:"Creating Rooms",permalink:"/ProceduralDungeon/3.3/guides/Creating-Rooms"},next:{title:"Choose First Room",permalink:"/ProceduralDungeon/3.3/guides/Choose-First-Room-Data"}},a={},l=[{value:"Create a dungeon generator",id:"create-a-dungeon-generator",level:2},{value:"Useful functions of the Dungeon Generator",id:"useful-functions-of-the-dungeon-generator",level:2},{value:"The <code>Rooms</code> variable, at the heart of the plugin",id:"the-rooms-variable-at-the-heart-of-the-plugin",level:2},{value:"Generator settings",id:"generator-settings",level:2},{value:"Use Generator Transform",id:"use-generator-transform",level:4},{value:"Generation Type",id:"generation-type",level:4},{value:"Can Loop",id:"can-loop",level:4},{value:"Seed Type",id:"seed-type",level:4},{value:"Seed",id:"seed",level:4},{value:"Seed Increment",id:"seed-increment",level:4},{value:"Use World Collision Checks",id:"use-world-collision-checks",level:4}];function h(e){const n={a:"a",br:"br",code:"code",em:"em",h1:"h1",h2:"h2",h4:"h4",header:"header",img:"img",li:"li",p:"p",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",ul:"ul",...(0,t.R)(),...e.components};return(0,r.jsxs)(r.Fragment,{children:[(0,r.jsx)(n.header,{children:(0,r.jsx)(n.h1,{id:"dungeon-generator",children:"Dungeon Generator"})}),"\n",(0,r.jsx)(n.h2,{id:"create-a-dungeon-generator",children:"Create a dungeon generator"}),"\n",(0,r.jsxs)(n.p,{children:["The dungeon generator is an actor placed in your main level. It is responsible to generate the dungeon and spawn the room levels.",(0,r.jsx)(n.br,{}),"\n",'To create one, create a new blueprint, expand "All Classes" and type ',(0,r.jsx)(n.code,{children:"DungeonGenerator"}),"."]}),"\n",(0,r.jsxs)(n.p,{children:["The dungeon generator don't work directly by itself, you have to ",(0,r.jsx)(n.a,{href:"Creating-Rooms",children:"create data and levels for your rooms"})," first."]}),"\n",(0,r.jsx)(n.p,{children:"To be able to generate a dungeon, you also have to override some minimal functions:"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsx)(n.li,{children:(0,r.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Choose-First-Room-Data",children:"Choose First Room Data"})}),"\n",(0,r.jsx)(n.li,{children:(0,r.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Choose-Next-Room-Data",children:"Choose Next Room Data"})}),"\n",(0,r.jsx)(n.li,{children:(0,r.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Continue-To-Add-Room",children:"Continue To Add Room"})}),"\n",(0,r.jsx)(n.li,{children:(0,r.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Is-Valid-Dungeon",children:"Is Valid Dungeon"})}),"\n",(0,r.jsx)(n.li,{children:(0,r.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Choose-Door",children:"Choose Door"})}),"\n"]}),"\n",(0,r.jsx)(n.p,{children:"There are some other functions you can override:"}),"\n",(0,r.jsxs)(n.table,{children:[(0,r.jsx)(n.thead,{children:(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.th,{children:"Function"}),(0,r.jsx)(n.th,{children:"Description"})]})}),(0,r.jsxs)(n.tbody,{children:[(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:(0,r.jsx)(n.strong,{children:"Pre\xa0Generation"})}),(0,r.jsx)(n.td,{children:"Called before anything else when the generator is about to generate a new dungeon."})]}),(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:(0,r.jsx)(n.strong,{children:"Post\xa0Generation"})}),(0,r.jsx)(n.td,{children:"Called after loading and initializing all rooms of the dungeon. Use it to spawn or teleport players in the first room."})]}),(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:(0,r.jsx)(n.strong,{children:"Generation\xa0Init"})}),(0,r.jsxs)(n.td,{children:["Called each time the generation start over when ",(0,r.jsx)(n.code,{children:"Is\xa0Valid\xa0Dungeon"})," returns false. Mostly used to reset all your custom generation variables."]})]}),(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:(0,r.jsx)(n.strong,{children:"Generation\xa0Failed"})}),(0,r.jsxs)(n.td,{children:["Called when ",(0,r.jsx)(n.code,{children:"Is\xa0Valid\xa0Dungeon"})," returned false after too many generation attempts. No dungeon is generated when this is called."]})]}),(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:(0,r.jsx)(n.strong,{children:"On\xa0Room\xa0Added"})}),(0,r.jsxs)(n.td,{children:["Called when a room is successfully added to the dungeon (but not spawned yet). The added room is destroyed when ",(0,r.jsx)(n.code,{children:"Is\xa0Valid\xa0Dungeon"})," returns false (you may have to use ",(0,r.jsx)(n.em,{children:"GenerationInit"})," to reset variables changed from this call)."]})]}),(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:(0,r.jsx)(n.strong,{children:"Initialize\xa0Dungeon"})}),(0,r.jsxs)(n.td,{children:["Called after ",(0,r.jsx)(n.code,{children:"Continue\xa0To\xa0Add\xa0Room"})," returned false. Allows you to initialize room instances before validating the dungeon with ",(0,r.jsx)(n.code,{children:"Is\xa0Valid\xa0Dungeon"})," (useful if you want for example locking some rooms and putting keys in other ones)."]})]})]})]}),"\n",(0,r.jsx)(n.h2,{id:"useful-functions-of-the-dungeon-generator",children:"Useful functions of the Dungeon Generator"}),"\n",(0,r.jsxs)(n.p,{children:["Aside the internal events you can override, there are some useful other functions the ",(0,r.jsx)(n.code,{children:"Dungeon Generator"})," provides to you.",(0,r.jsx)(n.br,{}),"\n","There are also the ",(0,r.jsx)(n.code,{children:"Generate"})," and the ",(0,r.jsx)(n.code,{children:"Unload"})," functions."]}),"\n",(0,r.jsxs)(n.p,{children:["Take a look at the ",(0,r.jsx)(n.a,{href:"https://benpyton.github.io/ProceduralDungeon/api/Classes/DungeonGenerator#nodes",children:"API Documentation"})," for a list of all functions provided by the ",(0,r.jsx)(n.code,{children:"Dungeon Generator"}),"."]}),"\n",(0,r.jsxs)(n.h2,{id:"the-rooms-variable-at-the-heart-of-the-plugin",children:["The ",(0,r.jsx)(n.code,{children:"Rooms"})," variable, at the heart of the plugin"]}),"\n",(0,r.jsxs)(n.p,{children:["The ",(0,r.jsx)(n.code,{children:"Rooms"})," variable of the ",(0,r.jsx)(n.code,{children:"Dungeon Generator"})," contains the list of all generated room instances.",(0,r.jsx)(n.br,{}),"\n","You can search for specific room instances, count them, check a path exists between rooms, etc.\r\nTake a look at the ",(0,r.jsx)(n.a,{href:"https://benpyton.github.io/ProceduralDungeon/api/Classes/DungeonGraph#nodes",children:"API Documentation"})," for a list of all functions provided by the ",(0,r.jsx)(n.code,{children:"Rooms"})," variable."]}),"\n",(0,r.jsx)(n.h2,{id:"generator-settings",children:"Generator settings"}),"\n",(0,r.jsxs)(n.p,{children:["The ",(0,r.jsx)(n.code,{children:"Dungeon Generator"})," actor has some parameters grouped inside the ",(0,r.jsx)(n.code,{children:"Procedural Generation"})," category:"]}),"\n",(0,r.jsx)(n.p,{children:(0,r.jsx)(n.img,{src:o(62463).A+"",width:"401",height:"271"})}),"\n",(0,r.jsx)(n.h4,{id:"use-generator-transform",children:"Use Generator Transform"}),"\n",(0,r.jsxs)(n.p,{children:["If checked, the dungeon will be offset and rotated using the dungeon generator actor's transform.",(0,r.jsx)(n.br,{}),"\n","If unchecked, the first room will be placed at world's (0,0,0) and no rotation will be applied."]}),"\n",(0,r.jsx)(n.h4,{id:"generation-type",children:"Generation Type"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsxs)(n.li,{children:["The ",(0,r.jsx)(n.code,{children:"depth first"})," will prioritize the last added room to continue the generation.\r\nThis will result in a dungeon more linear with shorter dead ends."]}),"\n",(0,r.jsxs)(n.li,{children:["The ",(0,r.jsx)(n.code,{children:"breadth first"})," will prioritize the first added room to continue the generation. This will result in a more spread dungeon."]}),"\n"]}),"\n",(0,r.jsx)(n.p,{children:"This is an example using the same very basic generation rules defined in this wiki. We just set a counter of 20 rooms to add the end room. So with depth first we have a lot of rooms to reach the end but dead-ends of only 1 room each. And with the breadth first, we have less rooms until the end, but the dead-ends are the same length of the path to reach the end (some dead-ends are shorter because the algorithm can't place a room after)."}),"\n",(0,r.jsx)(n.p,{children:(0,r.jsx)(n.img,{src:o(69432).A+"",width:"1158",height:"446"})}),"\n",(0,r.jsx)(n.h4,{id:"can-loop",children:"Can Loop"}),"\n",(0,r.jsxs)(n.p,{children:["If checked, when a new room is placed in the dungeon, any door aligned with another door of other rooms will be connected.",(0,r.jsx)(n.br,{}),"\n","If unchecked, only the door from the previous room (the one used to generate this room) will be connected.",(0,r.jsx)(n.br,{}),"\n",(0,r.jsxs)(n.em,{children:["This parameter is only used when ",(0,r.jsx)(n.code,{children:"Can Loop"})," from the project-wide ",(0,r.jsx)(n.a,{href:"/ProceduralDungeon/3.3/guides/Plugin-Settings",children:"settings"})," is also checked."]})]}),"\n",(0,r.jsx)(n.h4,{id:"seed-type",children:"Seed Type"}),"\n",(0,r.jsxs)(n.ul,{children:["\n",(0,r.jsxs)(n.li,{children:[(0,r.jsx)(n.code,{children:"Random"}),": at each generation, the seed is randomly chosen."]}),"\n",(0,r.jsxs)(n.li,{children:[(0,r.jsx)(n.code,{children:"Auto Increment"}),": the initial seed is used the first time, then a fixed number will be added at each generation. This will produce different dungeon at each generation, but you will have the same generation sequence each time you play."]}),"\n",(0,r.jsxs)(n.li,{children:[(0,r.jsx)(n.code,{children:"Fixed"}),": the initial seed is used each time you generate, so you will always get the same dungeon."]}),"\n"]}),"\n",(0,r.jsx)(n.h4,{id:"seed",children:"Seed"}),"\n",(0,r.jsxs)(n.p,{children:["The initial seed you want to use if the ",(0,r.jsx)(n.code,{children:"Seed Type"})," is not ",(0,r.jsx)(n.code,{children:"Random"})]}),"\n",(0,r.jsx)(n.h4,{id:"seed-increment",children:"Seed Increment"}),"\n",(0,r.jsxs)(n.p,{children:["The number used to increment the seed if the ",(0,r.jsx)(n.code,{children:"Seed Type"})," is ",(0,r.jsx)(n.code,{children:"Auto Increment"})]}),"\n",(0,r.jsx)(n.h4,{id:"use-world-collision-checks",children:"Use World Collision Checks"}),"\n",(0,r.jsxs)(n.p,{children:["When checked, the dungeon generator will test a box overlapping with world static actors in order to allow or not a room to be generated.",(0,r.jsx)(n.br,{}),"\n","This can be CPU-intensive and should be used only when necessary. Though, it is only used during the generation process, not during gameplay."]})]})}function c(e={}){const{wrapper:n}={...(0,t.R)(),...e.components};return n?(0,r.jsx)(n,{...e,children:(0,r.jsx)(h,{...e})}):h(e)}},69432:(e,n,o)=>{o.d(n,{A:()=>r});const r=o.p+"assets/images/DepthBreadth-9e141d8162efe2c6570472a20fa252de.jpg"},62463:(e,n,o)=>{o.d(n,{A:()=>r});const r=o.p+"assets/images/GeneratorSettings-fefbe25cc7ace01118dde8f227a96ad5.jpg"},28453:(e,n,o)=>{o.d(n,{R:()=>i,x:()=>d});var r=o(96540);const t={},s=r.createContext(t);function i(e){const n=r.useContext(s);return r.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function d(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:i(e.components),r.createElement(s.Provider,{value:n},e.children)}}}]);