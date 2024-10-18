"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[8414],{84731:(e,i,n)=>{n.r(i),n.d(i,{assets:()=>c,contentTitle:()=>l,default:()=>h,frontMatter:()=>s,metadata:()=>r,toc:()=>d});var o=n(74848),t=n(28453);const s={pagination_prev:null,pagination_next:null},l="Occlusion Culling",r={id:"guides/Occlusion-Culling",title:"Occlusion Culling",description:"The occlusion culling system of this plugin consists of showing only the necessary rooms to avoid a lots of mesh drawing and thus improving performances.",source:"@site/docs/guides/Occlusion-Culling.md",sourceDirName:"guides",slug:"/guides/Occlusion-Culling",permalink:"/ProceduralDungeon/guides/Occlusion-Culling",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},c={},d=[{value:"How the occlusion system works",id:"how-the-occlusion-system-works",level:2},{value:"Room Visibility Component",id:"room-visibility-component",level:2}];function a(e){const i={a:"a",admonition:"admonition",br:"br",code:"code",h1:"h1",h2:"h2",header:"header",li:"li",p:"p",ul:"ul",...(0,t.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsx)(i.header,{children:(0,o.jsx)(i.h1,{id:"occlusion-culling",children:"Occlusion Culling"})}),"\n",(0,o.jsx)(i.p,{children:"The occlusion culling system of this plugin consists of showing only the necessary rooms to avoid a lots of mesh drawing and thus improving performances."}),"\n",(0,o.jsx)(i.admonition,{type:"note",children:(0,o.jsxs)(i.p,{children:['This feature is badly named!\r\nThere is no real "occlusion" computation done, the room visibilities are toggled even when there is no occlusion of the room.',(0,o.jsx)(i.br,{}),"\n","I will rename that as ",(0,o.jsx)(i.code,{children:"Room Culling"})," instead in a future version of the plugin, this is more accurate."]})}),"\n",(0,o.jsx)(i.admonition,{type:"warning",children:(0,o.jsx)(i.p,{children:"Only the visibility of meshes is toggled. The ticking, animations, collisions, etc. are kept! So, the possible performance gain is on the rendering only!"})}),"\n",(0,o.jsxs)(i.p,{children:["You can toggle this system on or off in the ",(0,o.jsx)(i.a,{href:"/ProceduralDungeon/guides/Plugin-Settings",children:"Plugin Settings"})," alongside some other occlusion settings:"]}),"\n",(0,o.jsxs)(i.ul,{children:["\n",(0,o.jsxs)(i.li,{children:["The ",(0,o.jsx)(i.code,{children:"occlusion distance"})," determine how many rooms are visible from the player's room."]}),"\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Occlude dynamic actors"})," allows dynamic actors spawned after the dungeon generation and/or moving between rooms to be occluded with the rooms by the plugin. You'll also have to put a ",(0,o.jsx)(i.code,{children:"Room Visibility Component"})," on those actors."]}),"\n"]}),"\n",(0,o.jsx)(i.h2,{id:"how-the-occlusion-system-works",children:"How the occlusion system works"}),"\n",(0,o.jsxs)(i.p,{children:["The dungeon generator actor will keep track of the local player (",(0,o.jsx)(i.code,{children:"GetPlayerController(0)"}),") to know in which room it is located (referred later as the ",(0,o.jsx)(i.code,{children:"Current Player Room"}),")."]}),"\n",(0,o.jsxs)(i.p,{children:["Then, the generator actor will toggle the visibility state of the ",(0,o.jsx)(i.code,{children:"Current Player Room"})," and its directly connected rooms, and their own directly connected rooms, etc. based on the ",(0,o.jsx)(i.code,{children:"Occlusion Distance"})," in the ",(0,o.jsx)(i.a,{href:"/ProceduralDungeon/guides/Plugin-Settings",children:"Plugin Settings"})," (",(0,o.jsx)(i.code,{children:"1"})," is only the ",(0,o.jsx)(i.code,{children:"Current Player Room"}),", ",(0,o.jsx)(i.code,{children:"2"})," is its direct connected rooms, etc.)."]}),"\n",(0,o.jsxs)(i.p,{children:["Each room will then toggle the visibility of their owned actors (the ones placed directly in their level).",(0,o.jsx)(i.br,{}),"\n","If an actor is spawned during runtime, it will not belongs to any room level, and so their visibility will not be toggled.",(0,o.jsx)(i.br,{}),"\n","If an actor can move between rooms during the gameplay, you should not place it directly in the room level, as it will still belong to it and be occluded with that room instead of the one the actor is located in."]}),"\n",(0,o.jsx)(i.p,{children:'If you want to have actors that can travel across rooms, you should create a spawner actor placed in the room levels, which will spawn your "dynamic" actor that will then be not attached to a specific room.'}),"\n",(0,o.jsxs)(i.p,{children:["However, if you want this dynamic actor to be occluded properly with the rooms, you have to add a ",(0,o.jsx)(i.code,{children:"Room Visibility Component"})," on it (see below for more details on it)."]}),"\n",(0,o.jsx)(i.admonition,{type:"warning",children:(0,o.jsxs)(i.p,{children:[(0,o.jsx)(i.a,{href:"/ProceduralDungeon/guides/Geometry-Brushes",children:"Geometry Brushes"})," can't have there visibility toggled, and so can't be hidden by the occlusion culling system.\r\nYou should convert them into static meshes in order to make them occluded correctly with the rooms."]})}),"\n",(0,o.jsx)(i.h2,{id:"room-visibility-component",children:"Room Visibility Component"}),"\n",(0,o.jsx)(i.p,{children:"This actor component allows the actor to be tracked by the dungeon's rooms and so be occluded with them."}),"\n",(0,o.jsxs)(i.p,{children:["You need to toggle on ",(0,o.jsx)(i.code,{children:"Occlude Dynamic Actors"})," in the ",(0,o.jsx)(i.a,{href:"/ProceduralDungeon/guides/Plugin-Settings",children:"Plugin Settings"})," in order to make it working."]}),"\n",(0,o.jsx)(i.p,{children:"What is available from this component:"}),"\n",(0,o.jsxs)(i.ul,{children:["\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Is Visible"})," function returns true if the actor is inside a visible room."]}),"\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Set Visibility"})," allows you to set the visibility mode","\n",(0,o.jsxs)(i.ul,{children:["\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Same As Room"})," will toggle actor's visibility based on ",(0,o.jsx)(i.code,{children:"Is Visible"})," value."]}),"\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Force Hidden"}),"and ",(0,o.jsx)(i.code,{children:"Force Visible"})," will turn visibility on/off regardless the ",(0,o.jsx)(i.code,{children:"Is Visible"})," value."]}),"\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Custom"})," will does nothing itself and lets you manage the actor's visibility. You can then use ",(0,o.jsx)(i.code,{children:"Is Visible"})," and/or your own logic to toggle actor's visibility."]}),"\n"]}),"\n"]}),"\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"Get Visibility"})," will returns the current visibility mode set by ",(0,o.jsx)(i.code,{children:"Set Visibility"}),"."]}),"\n",(0,o.jsxs)(i.li,{children:[(0,o.jsx)(i.code,{children:"On Room Visibility Changed"})," is a delegate called when the ",(0,o.jsx)(i.code,{children:"Is Visible"})," value changes. Useful for example when you have a ",(0,o.jsx)(i.code,{children:"Custom"})," visibility and you are changing your actor visibility by events instead of in ",(0,o.jsx)(i.code,{children:"Tick"}),"."]}),"\n"]})]})}function h(e={}){const{wrapper:i}={...(0,t.R)(),...e.components};return i?(0,o.jsx)(i,{...e,children:(0,o.jsx)(a,{...e})}):a(e)}},28453:(e,i,n)=>{n.d(i,{R:()=>l,x:()=>r});var o=n(96540);const t={},s=o.createContext(t);function l(e){const i=o.useContext(s);return o.useMemo((function(){return"function"==typeof e?e(i):{...i,...e}}),[i,e])}function r(e){let i;return i=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:l(e.components),o.createElement(s.Provider,{value:i},e.children)}}}]);