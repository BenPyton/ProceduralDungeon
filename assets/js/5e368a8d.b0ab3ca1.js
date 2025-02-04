"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[9511],{85942:(e,i,n)=>{n.r(i),n.d(i,{assets:()=>d,contentTitle:()=>o,default:()=>h,frontMatter:()=>l,metadata:()=>s,toc:()=>a});const s=JSON.parse('{"id":"api/Classes/StaticRoomVisibilityComponent/Nodes/IsVisible/IsVisible","title":"IsVisible","description":"Procedural Dungeon \u23f5","source":"@site/versioned_docs/version-3.3/api/Classes/StaticRoomVisibilityComponent/Nodes/IsVisible/IsVisible.md","sourceDirName":"api/Classes/StaticRoomVisibilityComponent/Nodes/IsVisible","slug":"/api/StaticRoomVisibilityComponent/IsVisible","permalink":"/ProceduralDungeon/3.3/api/StaticRoomVisibilityComponent/IsVisible","draft":false,"unlisted":false,"tags":[],"version":"3.3","frontMatter":{"slug":"/api/StaticRoomVisibilityComponent/IsVisible","pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var t=n(74848),r=n(28453);const l={slug:"/api/StaticRoomVisibilityComponent/IsVisible",pagination_prev:null,pagination_next:null},o="Is In Visible Room",d={},a=[{value:"Description",id:"description",level:2},{value:"Inputs",id:"inputs",level:2},{value:"Outputs",id:"outputs",level:2}];function c(e){const i={a:"a",code:"code",h1:"h1",h2:"h2",header:"header",img:"img",p:"p",pre:"pre",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,r.R)(),...e.components};return(0,t.jsxs)(t.Fragment,{children:[(0,t.jsxs)(i.p,{children:[(0,t.jsx)(i.a,{href:"/ProceduralDungeon/3.3/api/",children:"Procedural Dungeon"})," \u23f5\r\n",(0,t.jsx)(i.a,{href:"/ProceduralDungeon/3.3/api/Classes/StaticRoomVisibilityComponent/",children:"Room Visibility (Static)"})," \u23f5\r\nIs In Visible Room"]}),"\n",(0,t.jsx)(i.header,{children:(0,t.jsx)(i.h1,{id:"is-in-visible-room",children:"Is In Visible Room"})}),"\n",(0,t.jsxs)(i.p,{children:[(0,t.jsx)(i.strong,{children:"Category:"})," Procedural\xa0Dungeon"]}),"\n",(0,t.jsx)(i.h2,{id:"description",children:"Description"}),"\n",(0,t.jsxs)(i.p,{children:["Returns true if the actor is in a visible room.",(0,t.jsx)("br",{}),'Always returns true when "Occlude Dynamic Actors" is unchecked in the plugin\'s settings',(0,t.jsx)("br",{}),'Useful with "Custom" visibility.']}),"\n",(0,t.jsx)(i.p,{children:"Node"}),"\n",(0,t.jsx)(i.p,{children:(0,t.jsx)(i.img,{src:n(30033).A+"",width:"233",height:"70"})}),"\n",(0,t.jsx)(i.p,{children:"C++"}),"\n",(0,t.jsx)(i.pre,{children:(0,t.jsx)(i.code,{className:"language-cpp",children:"bool IsVisible()\n"})}),"\n",(0,t.jsx)(i.h2,{id:"inputs",children:"Inputs"}),"\n",(0,t.jsxs)(i.table,{children:[(0,t.jsx)(i.thead,{children:(0,t.jsxs)(i.tr,{children:[(0,t.jsx)(i.th,{children:"Name"}),(0,t.jsx)(i.th,{children:"Type"}),(0,t.jsx)(i.th,{children:"Description"})]})}),(0,t.jsx)(i.tbody,{children:(0,t.jsxs)(i.tr,{children:[(0,t.jsx)(i.td,{children:"Target"}),(0,t.jsx)(i.td,{children:"Room Visibility (Static) Object Reference"}),(0,t.jsx)(i.td,{})]})})]}),"\n",(0,t.jsx)(i.h2,{id:"outputs",children:"Outputs"}),"\n",(0,t.jsxs)(i.table,{children:[(0,t.jsx)(i.thead,{children:(0,t.jsxs)(i.tr,{children:[(0,t.jsx)(i.th,{children:"Name"}),(0,t.jsx)(i.th,{children:"Type"}),(0,t.jsx)(i.th,{children:"Description"})]})}),(0,t.jsx)(i.tbody,{children:(0,t.jsxs)(i.tr,{children:[(0,t.jsx)(i.td,{children:"Return Value"}),(0,t.jsx)(i.td,{children:"Boolean"}),(0,t.jsxs)(i.td,{children:["Returns true if the actor is in a visible room.",(0,t.jsx)("br",{}),'Always returns true when "Occlude Dynamic Actors" is unchecked in the plugin\'s settings',(0,t.jsx)("br",{}),'Useful with "Custom" visibility.']})]})})]})]})}function h(e={}){const{wrapper:i}={...(0,r.R)(),...e.components};return i?(0,t.jsx)(i,{...e,children:(0,t.jsx)(c,{...e})}):c(e)}},30033:(e,i,n)=>{n.d(i,{A:()=>s});const s="data:image/png;base64,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"},28453:(e,i,n)=>{n.d(i,{R:()=>l,x:()=>o});var s=n(96540);const t={},r=s.createContext(t);function l(e){const i=s.useContext(r);return s.useMemo((function(){return"function"==typeof e?e(i):{...i,...e}}),[i,e])}function o(e){let i;return i=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:l(e.components),s.createElement(r.Provider,{value:i},e.children)}}}]);