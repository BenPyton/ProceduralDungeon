"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[6045],{52870:(e,o,n)=>{n.r(o),n.d(o,{assets:()=>a,contentTitle:()=>s,default:()=>h,frontMatter:()=>i,metadata:()=>c,toc:()=>l});var t=n(74848),r=n(28453);const i={slug:"/guides/Door",pagination_prev:null,pagination_next:null},s="Door",c={id:"guides/Getting-Started/Door",title:"Door",description:"The Door class is the base class for all doors spawned by the dungeon.",source:"@site/docs/guides/Getting-Started/Door.md",sourceDirName:"guides/Getting-Started",slug:"/guides/Door",permalink:"/ProceduralDungeon/guides/Door",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/guides/Door",pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},a={},l=[{value:"Trigger Doors",id:"trigger-doors",level:3},{value:"Multiplayer",id:"multiplayer",level:3},{value:"Good Practice",id:"good-practice",level:3}];function d(e){const o={a:"a",br:"br",code:"code",em:"em",h1:"h1",h3:"h3",header:"header",hr:"hr",img:"img",li:"li",p:"p",strong:"strong",ul:"ul",...(0,r.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsx)(o.header,{children:(0,t.jsx)(o.h1,{id:"door",children:"Door"})}),"\n",(0,t.jsxs)(o.p,{children:["The ",(0,t.jsx)(o.code,{children:"Door"})," class is the base class for all doors spawned by the dungeon."]}),"\n",(0,t.jsxs)(o.p,{children:["You have to manage yourself how your doors open or close.",(0,t.jsx)(o.br,{}),"\n","For that, you just need to create a child blueprint class of ",(0,t.jsx)(o.code,{children:"Door"})," and call the ",(0,t.jsx)(o.code,{children:"Open"}),"  function whenever you want to open or close the door.",(0,t.jsx)(o.br,{}),"\n","You then can do what you want when a door opens or closes by overriding the events ",(0,t.jsx)(o.code,{children:"OnOpen"})," and ",(0,t.jsx)(o.code,{children:"OnClose"})," (like playing animation, sounds, effects, etc.)."]}),"\n",(0,t.jsxs)(o.p,{children:["If you want to place walls instead of doors when the rooms are not connected, you still need to inherits your blueprint from the ",(0,t.jsx)(o.code,{children:"Door"})," class, but with no logic inside and just a wall static mesh."]}),"\n",(0,t.jsxs)(o.p,{children:["You can lock or unlock a door to prevent players from opening the door if you want. You just need to call the ",(0,t.jsx)(o.code,{children:"Lock"})," function for that."]}),"\n",(0,t.jsxs)(o.p,{children:["If you want to see the correct size depending on the ",(0,t.jsx)(o.a,{href:"/ProceduralDungeon/guides/Door-Types",children:"door type"})," in the blueprint viewport of the actor, you have to set the ",(0,t.jsx)(o.code,{children:"Type"})," variable of the door."]}),"\n",(0,t.jsx)(o.h3,{id:"trigger-doors",children:"Trigger Doors"}),"\n",(0,t.jsxs)(o.p,{children:["The ",(0,t.jsx)(o.code,{children:"TriggerDoor"})," class is a ",(0,t.jsx)(o.code,{children:"Door"})," which will open or close depending of actors being inside its trigger box."]}),"\n",(0,t.jsxs)(o.p,{children:["By default, all ",(0,t.jsx)(o.code,{children:"Character"})," actors trigger the door, but you can change this behavior by overriding the ",(0,t.jsx)(o.code,{children:"IsValidActor"})," function."]}),"\n",(0,t.jsx)(o.p,{children:(0,t.jsx)(o.img,{src:n(56187).A+"",width:"661",height:"217"})}),"\n",(0,t.jsx)(o.h3,{id:"multiplayer",children:"Multiplayer"}),"\n",(0,t.jsxs)(o.p,{children:["The open/close state of the doors are replicated over the network.",(0,t.jsx)(o.br,{}),"\n","However, you'll need to do your game network logic correctly to make doors work properly in multiplayer."]}),"\n",(0,t.jsxs)(o.p,{children:["For example, if you want to make a door that open/close on a player key press, you'll need to send an RPC from your ",(0,t.jsx)(o.code,{children:"PlayerController"})," or your controlled ",(0,t.jsx)(o.code,{children:"Pawn"})," in order to call your interaction function on you door to open/close it.",(0,t.jsx)(o.br,{}),"\n",(0,t.jsx)(o.em,{children:'(The screen below uses a generic interface to interact with any interactable actor. See the "Good Practice" section below on how to do it)'})]}),"\n",(0,t.jsx)(o.p,{children:(0,t.jsx)(o.img,{src:n(7693).A+"",width:"1060",height:"577"})}),"\n",(0,t.jsx)(o.hr,{}),"\n",(0,t.jsx)(o.h3,{id:"good-practice",children:"Good Practice"}),"\n",(0,t.jsxs)(o.p,{children:["The screen above uses a custom interface called ",(0,t.jsx)(o.code,{children:"IInteractable"})," to generalized the player interaction with whatever actor you want in the game."]}),"\n",(0,t.jsxs)(o.p,{children:[(0,t.jsxs)(o.strong,{children:["This section is not specific to the ",(0,t.jsx)(o.code,{children:"Door"})," class of this plugin"]}),", but I will explain briefly how to use an interface in Unreal blueprint for those who are not familiar with this concept."]}),"\n",(0,t.jsxs)(o.p,{children:["First, you have to create the interface class and populate it with some functions. For the example, I called it ",(0,t.jsx)(o.code,{children:"IInteractable"})," (the first ",(0,t.jsx)(o.code,{children:"I"})," is a naming convention for interfaces) and I put two functions inside it:"]}),"\n",(0,t.jsxs)(o.ul,{children:["\n",(0,t.jsxs)(o.li,{children:[(0,t.jsx)(o.code,{children:"CanInteract"})," which takes an ",(0,t.jsx)(o.code,{children:"Actor"})," (named ",(0,t.jsx)(o.code,{children:"Caller"}),") in input if I need to check some things before interacting, and returns a ",(0,t.jsx)(o.code,{children:"bool"})," (true if ",(0,t.jsx)(o.code,{children:"Caller"}),"is able to interact)."]}),"\n",(0,t.jsxs)(o.li,{children:[(0,t.jsx)(o.code,{children:"Interact"})," which takes also the ",(0,t.jsx)(o.code,{children:"Caller"})," actor as input, and do the actual interaction on the ",(0,t.jsx)(o.code,{children:"IInteractable"})," actor."]}),"\n"]}),"\n",(0,t.jsxs)(o.p,{children:["Remember that you can create ",(0,t.jsx)(o.em,{children:"any"})," function with ",(0,t.jsx)(o.em,{children:"any"})," inputs and outputs you want in the interface."]}),"\n",(0,t.jsx)(o.p,{children:"Then you have to add the new interface in your interactable actor settings (the door in my example)."}),"\n",(0,t.jsxs)(o.p,{children:["Then, you implement the ",(0,t.jsx)(o.code,{children:"CanInteract"})," and the ",(0,t.jsx)(o.code,{children:"Interact"})," functions from the interface in your actor."]})]})}function h(e={}){const{wrapper:o}={...(0,r.R)(),...e.components};return o?(0,t.jsx)(o,{...e,children:(0,t.jsx)(d,{...e})}):d(e)}},56187:(e,o,n)=>{n.d(o,{A:()=>t});const t=n.p+"assets/images/Door_IsValidActor-290edff7506034767e9f0bfd77622fb5.jpg"},7693:(e,o,n)=>{n.d(o,{A:()=>t});const t=n.p+"assets/images/Door_MultiInteract-cae4b50339b236294b43714e91332bfe.jpg"},28453:(e,o,n)=>{n.d(o,{R:()=>s,x:()=>c});var t=n(96540);const r={},i=t.createContext(r);function s(e){const o=t.useContext(i);return t.useMemo((function(){return"function"==typeof e?e(o):{...o,...e}}),[o,e])}function c(e){let o;return o=e.disableParentContext?"function"==typeof e.components?e.components(r):e.components||r:s(e.components),t.createElement(i.Provider,{value:o},e.children)}}}]);