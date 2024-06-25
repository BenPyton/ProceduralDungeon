"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[4353],{4199:(e,n,t)=>{t.r(n),t.d(n,{assets:()=>d,contentTitle:()=>o,default:()=>u,frontMatter:()=>i,metadata:()=>l,toc:()=>s});var r=t(4848),a=t(8453);const i={slug:"/api/DungeonBlueprintLibrary/CompareDataTableRows"},o="Equal (Data Table Row Handle)",l={id:"api/DungeonBlueprintLibrary/nodes/CompareDataTableRows",title:"CompareDataTableRows",description:"Procedural Dungeon >",source:"@site/docs/api/DungeonBlueprintLibrary/nodes/CompareDataTableRows.md",sourceDirName:"api/DungeonBlueprintLibrary/nodes",slug:"/api/DungeonBlueprintLibrary/CompareDataTableRows",permalink:"/ProceduralDungeon/api/DungeonBlueprintLibrary/CompareDataTableRows",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/api/DungeonBlueprintLibrary/CompareDataTableRows"},sidebar:"apiSidebar",previous:{title:"DungeonBlueprintLibrary",permalink:"/ProceduralDungeon/api/DungeonBlueprintLibrary/"},next:{title:"DoorDirection_Add",permalink:"/ProceduralDungeon/api/DungeonBlueprintLibrary/DoorDirection_Add"}},d={},s=[{value:"Inputs",id:"inputs",level:2},{value:"Outputs",id:"outputs",level:2}];function c(e){const n={a:"a",code:"code",h1:"h1",h2:"h2",img:"img",p:"p",pre:"pre",table:"table",tbody:"tbody",td:"td",th:"th",thead:"thead",tr:"tr",...(0,a.R)(),...e.components};return(0,r.jsxs)(r.Fragment,{children:[(0,r.jsxs)(n.p,{children:[(0,r.jsx)(n.a,{href:"/ProceduralDungeon/api/",children:"Procedural Dungeon"})," >\n",(0,r.jsx)(n.a,{href:"/ProceduralDungeon/api/DungeonBlueprintLibrary/",children:"Dungeon Blueprint Library"})," >\nEqual (Data Table Row Handle)"]}),"\n",(0,r.jsx)(n.h1,{id:"equal-data-table-row-handle",children:"Equal (Data Table Row Handle)"}),"\n",(0,r.jsx)(n.p,{children:"Node"}),"\n",(0,r.jsx)(n.p,{children:(0,r.jsx)(n.img,{src:t(8245).A+"",width:"143",height:"60"})}),"\n",(0,r.jsx)(n.p,{children:"C++"}),"\n",(0,r.jsx)(n.pre,{children:(0,r.jsx)(n.code,{className:"language-cpp",children:"bool CompareDataTableRows(const FDataTableRowHandle& A, const FDataTableRowHandle& B)\n"})}),"\n",(0,r.jsx)(n.h2,{id:"inputs",children:"Inputs"}),"\n",(0,r.jsxs)(n.table,{children:[(0,r.jsx)(n.thead,{children:(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.th,{children:"Name"}),(0,r.jsx)(n.th,{children:"Type"}),(0,r.jsx)(n.th,{children:"Description"})]})}),(0,r.jsxs)(n.tbody,{children:[(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:"A"}),(0,r.jsx)(n.td,{children:"Data Table Row Handle Structure (by ref)"}),(0,r.jsx)(n.td,{})]}),(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:"B"}),(0,r.jsx)(n.td,{children:"Data Table Row Handle Structure (by ref)"}),(0,r.jsx)(n.td,{})]})]})]}),"\n",(0,r.jsx)(n.h2,{id:"outputs",children:"Outputs"}),"\n",(0,r.jsxs)(n.table,{children:[(0,r.jsx)(n.thead,{children:(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.th,{children:"Name"}),(0,r.jsx)(n.th,{children:"Type"}),(0,r.jsx)(n.th,{children:"Description"})]})}),(0,r.jsx)(n.tbody,{children:(0,r.jsxs)(n.tr,{children:[(0,r.jsx)(n.td,{children:"Return Value"}),(0,r.jsx)(n.td,{children:"Boolean"}),(0,r.jsx)(n.td,{children:"Compare Data Table Rows"})]})})]})]})}function u(e={}){const{wrapper:n}={...(0,a.R)(),...e.components};return n?(0,r.jsx)(n,{...e,children:(0,r.jsx)(c,{...e})}):c(e)}},8245:(e,n,t)=>{t.d(n,{A:()=>r});const r="data:image/png;base64,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"},8453:(e,n,t)=>{t.d(n,{R:()=>o,x:()=>l});var r=t(6540);const a={},i=r.createContext(a);function o(e){const n=r.useContext(i);return r.useMemo((function(){return"function"==typeof e?e(n):{...n,...e}}),[n,e])}function l(e){let n;return n=e.disableParentContext?"function"==typeof e.components?e.components(a):e.components||a:o(e.components),r.createElement(i.Provider,{value:n},e.children)}}}]);