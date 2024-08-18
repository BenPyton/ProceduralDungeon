"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[4396],{4497:(n,e,t)=>{t.r(e),t.d(e,{assets:()=>a,contentTitle:()=>d,default:()=>c,frontMatter:()=>o,metadata:()=>i,toc:()=>u});var r=t(4848),s=t(8453);const o={slug:"/api/DungeonGraph/Count",pagination_prev:null,pagination_next:null},d="Count",i={id:"api/Classes/DungeonGraph/Nodes/Count/Count",title:"Count",description:"Procedural Dungeon \u23f5",source:"@site/docs/api/Classes/DungeonGraph/Nodes/Count/Count.md",sourceDirName:"api/Classes/DungeonGraph/Nodes/Count",slug:"/api/DungeonGraph/Count",permalink:"/ProceduralDungeon/api/DungeonGraph/Count",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/api/DungeonGraph/Count",pagination_prev:null,pagination_next:null},sidebar:"homeSidebar"},a={},u=[{value:"Description",id:"description",level:2},{value:"Inputs",id:"inputs",level:2},{value:"Outputs",id:"outputs",level:2}];function h(n){const e={a:"a",code:"code",h1:"h1",h2:"h2",img:"img",p:"p",pre:"pre",strong:"strong",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,s.R)(),...n.components};return(0,r.jsxs)(r.Fragment,{children:[(0,r.jsxs)(e.p,{children:[(0,r.jsx)(e.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," \u23f5\n",(0,r.jsx)(e.a,{href:"/ProceduralDungeon/api/Classes/DungeonGraph/",children:"Dungeon Graph"})," \u23f5\nCount"]}),"\n",(0,r.jsx)(e.h1,{id:"count",children:"Count"}),"\n",(0,r.jsxs)(e.p,{children:[(0,r.jsx)(e.strong,{children:"Category:"})," Dungeon\xa0Graph"]}),"\n",(0,r.jsx)(e.h2,{id:"description",children:"Description"}),"\n",(0,r.jsx)(e.p,{children:"Returns the total number of room"}),"\n",(0,r.jsx)(e.p,{children:"Node"}),"\n",(0,r.jsx)(e.p,{children:(0,r.jsx)(e.img,{src:t(9541).A+"",width:"208",height:"38"})}),"\n",(0,r.jsx)(e.p,{children:"C++"}),"\n",(0,r.jsx)(e.pre,{children:(0,r.jsx)(e.code,{className:"language-cpp",children:"int32 Count() const\n"})}),"\n",(0,r.jsx)(e.h2,{id:"inputs",children:"Inputs"}),"\n",(0,r.jsxs)(e.table,{children:[(0,r.jsx)(e.thead,{children:(0,r.jsxs)(e.tr,{children:[(0,r.jsx)(e.th,{children:"Name"}),(0,r.jsx)(e.th,{children:"Type"}),(0,r.jsx)(e.th,{children:"Description"})]})}),(0,r.jsx)(e.tbody,{children:(0,r.jsxs)(e.tr,{children:[(0,r.jsx)(e.td,{children:"Target"}),(0,r.jsx)(e.td,{children:"Dungeon Graph Object Reference"}),(0,r.jsx)(e.td,{})]})})]}),"\n",(0,r.jsx)(e.h2,{id:"outputs",children:"Outputs"}),"\n",(0,r.jsxs)(e.table,{children:[(0,r.jsx)(e.thead,{children:(0,r.jsxs)(e.tr,{children:[(0,r.jsx)(e.th,{children:"Name"}),(0,r.jsx)(e.th,{children:"Type"}),(0,r.jsx)(e.th,{children:"Description"})]})}),(0,r.jsx)(e.tbody,{children:(0,r.jsxs)(e.tr,{children:[(0,r.jsx)(e.td,{children:"Return Value"}),(0,r.jsx)(e.td,{children:"Integer"}),(0,r.jsx)(e.td,{children:"Returns the total number of room"})]})})]})]})}function c(n={}){const{wrapper:e}={...(0,s.R)(),...n.components};return e?(0,r.jsx)(e,{...n,children:(0,r.jsx)(h,{...n})}):h(n)}},9541:(n,e,t)=>{t.d(e,{A:()=>r});const r="data:image/png;base64,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"},8453:(n,e,t)=>{t.d(e,{R:()=>d,x:()=>i});var r=t(6540);const s={},o=r.createContext(s);function d(n){const e=r.useContext(o);return r.useMemo((function(){return"function"==typeof n?n(e):{...e,...n}}),[e,n])}function i(n){let e;return e=n.disableParentContext?"function"==typeof n.components?n.components(s):n.components||s:d(n.components),r.createElement(o.Provider,{value:e},n.children)}}}]);