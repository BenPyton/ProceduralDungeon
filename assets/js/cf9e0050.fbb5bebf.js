"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[14841],{19125:(e,o,i)=>{i.r(o),i.d(o,{assets:()=>a,contentTitle:()=>c,default:()=>l,frontMatter:()=>r,metadata:()=>n,toc:()=>d});const n=JSON.parse('{"id":"guides/Advanced-Features/Room-Visitor","title":"Room Visitor","description":"The Room Visitor is an interface used to add behaviors to your actors or components when they enter or exit a room.","source":"@site/versioned_docs/version-3.4/guides/Advanced-Features/Room-Visitor.md","sourceDirName":"guides/Advanced-Features","slug":"/guides/Room-Visitor","permalink":"/ProceduralDungeon/3.4/guides/Room-Visitor","draft":false,"unlisted":false,"tags":[],"version":"3.4","frontMatter":{"slug":"/guides/Room-Visitor","pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var t=i(74848),s=i(28453);const r={slug:"/guides/Room-Visitor",pagination_prev:null,pagination_next:null},c="Room Visitor",a={},d=[{value:"How to use it",id:"how-to-use-it",level:2},{value:"Differences with <code>Room Observer</code> component",id:"differences-with-room-observer-component",level:2},{value:"Differences with <code>Room Visibility</code> component",id:"differences-with-room-visibility-component",level:2}];function h(e){const o={a:"a",code:"code",h1:"h1",h2:"h2",header:"header",img:"img",p:"p",strong:"strong",...(0,s.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsx)(o.header,{children:(0,t.jsx)(o.h1,{id:"room-visitor",children:"Room Visitor"})}),"\n",(0,t.jsxs)(o.p,{children:["The ",(0,t.jsx)(o.code,{children:"Room Visitor"})," is an ",(0,t.jsx)(o.a,{href:"https://dev.epicgames.com/community/learning/tutorials/bLXe/interfaces-bp-c",children:"interface"})," used to add behaviors to your actors or components when they enter or exit a room."]}),"\n",(0,t.jsxs)(o.p,{children:["For example, you can use this interface to keep track of the rooms the actor is in to be able to call functions on them, change some ",(0,t.jsx)(o.a,{href:"/ProceduralDungeon/3.4/guides/Room-Custom-Data",children:"Room Custom Data"})," variables, display various details on player's HUD, etc."]}),"\n",(0,t.jsxs)(o.p,{children:["The ",(0,t.jsx)(o.a,{href:"/ProceduralDungeon/3.4/guides/Occlusion-Culling",children:"Room Visibility Component"})," (used to show/hide an actor with the rooms it is in) is implementing the ",(0,t.jsx)(o.code,{children:"Room Visitor"})," interface to bind/unbind to the room's visibility event dispatcher when the actor enters or exits any room, and thus can change its visibility according to the room visibility state."]}),"\n",(0,t.jsx)(o.h2,{id:"how-to-use-it",children:"How to use it"}),"\n",(0,t.jsx)(o.p,{children:"Just add the interface like any other interface in Blueprint or C++, and implement its functions."}),"\n",(0,t.jsxs)(o.p,{children:["For example, this is a simple implementation in Blueprint to keep a reference of the rooms an actor is in (stored in an array called ",(0,t.jsx)(o.code,{children:"Cached Room Array"}),")."]}),"\n",(0,t.jsx)(o.p,{children:(0,t.jsx)(o.img,{src:i(16357).A+"",width:"1560",height:"320"})}),"\n",(0,t.jsxs)(o.h2,{id:"differences-with-room-observer-component",children:["Differences with ",(0,t.jsx)(o.a,{href:"/ProceduralDungeon/3.4/guides/Room-Observer",children:(0,t.jsx)(o.code,{children:"Room Observer"})})," component"]}),"\n",(0,t.jsxs)(o.p,{children:["As the name implies, the ",(0,t.jsx)(o.code,{children:"Room Visitor"}),' interface is used to create game logic when the actor "',(0,t.jsx)(o.strong,{children:"visits"}),'" a new room.']}),"\n",(0,t.jsxs)(o.p,{children:["The implemented events are called only when the implementing actor enters/exits a room, ",(0,t.jsx)(o.strong,{children:"not"})," when other actors enter/exit rooms."]}),"\n",(0,t.jsxs)(o.p,{children:["On the other hand, the ",(0,t.jsx)(o.code,{children:"Room Observer"})," dispatch its events when ",(0,t.jsx)(o.strong,{children:"other"})," actors enter/leave the room the observer is in."]}),"\n",(0,t.jsxs)(o.h2,{id:"differences-with-room-visibility-component",children:["Differences with ",(0,t.jsx)(o.a,{href:"/ProceduralDungeon/3.4/guides/Occlusion-Culling",children:(0,t.jsx)(o.code,{children:"Room Visibility"})})," component"]}),"\n",(0,t.jsxs)(o.p,{children:["The ",(0,t.jsx)(o.code,{children:"Room Visibility"})," component ",(0,t.jsx)(o.strong,{children:"implements"})," the ",(0,t.jsx)(o.code,{children:"Room Visitor"})," interface, so it can bind/unbind its internal functions to the visibility event of the rooms the actor enters/exits."]}),"\n",(0,t.jsx)(o.p,{children:"This is a simple example of what you can do with this interface."})]})}function l(e={}){const{wrapper:o}={...(0,s.R)(),...e.components};return o?(0,t.jsx)(o,{...e,children:(0,t.jsx)(h,{...e})}):h(e)}},16357:(e,o,i)=>{i.d(o,{A:()=>n});const n=i.p+"assets/images/RoomVisitor-3ba55a29440bdb14785cd12cbc399ba0.png"},28453:(e,o,i)=>{i.d(o,{R:()=>r,x:()=>c});var n=i(96540);const t={},s=n.createContext(t);function r(e){const o=n.useContext(s);return n.useMemo((function(){return"function"==typeof e?e(o):{...o,...e}}),[o,e])}function c(e){let o;return o=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:r(e.components),n.createElement(s.Provider,{value:o},e.children)}}}]);