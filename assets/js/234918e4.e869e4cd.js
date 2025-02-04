"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[94571],{80590:(e,t,n)=>{n.r(t),n.d(t,{assets:()=>a,contentTitle:()=>c,default:()=>h,frontMatter:()=>l,metadata:()=>r,toc:()=>u});const r=JSON.parse('{"id":"contributing/index","title":"index","description":"Support me!","source":"@site/docs/contributing/index.md","sourceDirName":"contributing","slug":"/contributing","permalink":"/ProceduralDungeon/contributing","draft":false,"unlisted":false,"tags":[],"version":"current","frontMatter":{"slug":"/contributing","title":"index","hide_table_of_contents":true,"pagination_prev":null,"pagination_next":null},"sidebar":"homeSidebar"}');var o=n(74848),i=n(28453),s=n(17473);const l={slug:"/contributing",title:"index",hide_table_of_contents:!0,pagination_prev:null,pagination_next:null},c="Contributing to the Documentation",a={},u=[{value:"<em>Support me!</em>",id:"support-me",level:2}];function d(e){const t={a:"a",br:"br",em:"em",h1:"h1",h2:"h2",header:"header",img:"img",p:"p",strong:"strong",...(0,i.R)(),...e.components};return(0,o.jsxs)(o.Fragment,{children:[(0,o.jsx)(t.header,{children:(0,o.jsx)(t.h1,{id:"contributing-to-the-documentation",children:"Contributing to the Documentation"})}),"\n",(0,o.jsx)(s.A,{}),"\n",(0,o.jsx)(t.h2,{id:"support-me",children:(0,o.jsx)(t.em,{children:"Support me!"})}),"\n",(0,o.jsxs)(t.p,{children:["If you like my work, please consider ",(0,o.jsx)(t.strong,{children:"tipping"}),"!",(0,o.jsx)(t.br,{}),"\n","It will show me how it matters and allows me to ",(0,o.jsx)(t.strong,{children:"continue working"})," on those projects!"]}),"\n",(0,o.jsxs)(t.p,{children:[(0,o.jsx)(t.a,{href:"https://ko-fi.com/M4M3NW2JV",children:(0,o.jsx)(t.img,{src:"https://img.shields.io/badge/Ko--fi-FF5E5B?logo=kofi&logoColor=fff&style=for-the-badge",alt:"Ko-fi"})}),"\r\n",(0,o.jsx)(t.a,{href:"https://liberapay.com/BenPyton/donate",children:(0,o.jsx)(t.img,{src:"https://img.shields.io/badge/Liberapay-F6C915?logo=liberapay&logoColor=000&style=for-the-badge",alt:"Liberapay"})}),"\r\n",(0,o.jsx)(t.a,{href:"https://www.paypal.com/donate/?hosted_button_id=9VWP66JU5DZXN",children:(0,o.jsx)(t.img,{src:"https://img.shields.io/badge/PayPal-003087?logo=paypal&logoColor=fff&style=for-the-badge",alt:"PayPal"})})]})]})}function h(e={}){const{wrapper:t}={...(0,i.R)(),...e.components};return t?(0,o.jsx)(t,{...e,children:(0,o.jsx)(d,{...e})}):d(e)}},17473:(e,t,n)=>{n.d(t,{A:()=>k});var r=n(96540),o=n(34164),i=n(93751),s=n(56289),l=n(40797);const c=["zero","one","two","few","many","other"];function a(e){return c.filter((t=>e.includes(t)))}const u={locale:"en",pluralForms:a(["one","other"]),select:e=>1===e?"one":"other"};function d(){const{i18n:{currentLocale:e}}=(0,l.A)();return(0,r.useMemo)((()=>{try{return function(e){const t=new Intl.PluralRules(e);return{locale:e,pluralForms:a(t.resolvedOptions().pluralCategories),select:e=>t.select(e)}}(e)}catch(t){return console.error(`Failed to use Intl.PluralRules for locale "${e}".\nDocusaurus will fallback to the default (English) implementation.\nError: ${t.message}\n`),u}}),[e])}function h(){const e=d();return{selectMessage:(t,n)=>function(e,t,n){const r=e.split("|");if(1===r.length)return r[0];r.length>n.pluralForms.length&&console.error(`For locale=${n.locale}, a maximum of ${n.pluralForms.length} plural forms are expected (${n.pluralForms.join(",")}), but the message contains ${r.length}: ${e}`);const o=n.select(t),i=n.pluralForms.indexOf(o);return r[Math.min(i,r.length-1)]}(n,t,e)}}var m=n(22887),p=n(50539),f=n(9303);const g={cardContainer:"cardContainer_fWXF",cardTitle:"cardTitle_rnsV",cardDescription:"cardDescription_PWke"};var x=n(74848);function b(e){let{href:t,children:n}=e;return(0,x.jsx)(s.A,{href:t,className:(0,o.A)("card padding--lg",g.cardContainer),children:n})}function j(e){let{href:t,icon:n,title:r,description:i}=e;return(0,x.jsxs)(b,{href:t,children:[(0,x.jsxs)(f.A,{as:"h2",className:(0,o.A)("text--truncate",g.cardTitle),title:r,children:[n," ",r]}),i&&(0,x.jsx)("p",{className:(0,o.A)("text--truncate",g.cardDescription),title:i,children:i})]})}function y(e){let{item:t}=e;const n=(0,i.Nr)(t),r=function(){const{selectMessage:e}=h();return t=>e(t,(0,p.T)({message:"1 item|{count} items",id:"theme.docs.DocCard.categoryDescription.plurals",description:"The default description for a category card in the generated index about how many items this category includes"},{count:t}))}();return n?(0,x.jsx)(j,{href:n,icon:"\ud83d\uddc3\ufe0f",title:t.label,description:t.description??r(t.items.length)}):null}function w(e){let{item:t}=e;const n=(0,m.A)(t.href)?"\ud83d\udcc4\ufe0f":"\ud83d\udd17",r=(0,i.cC)(t.docId??void 0);return(0,x.jsx)(j,{href:t.href,icon:n,title:t.label,description:t.description??r?.description})}function _(e){let{item:t}=e;switch(t.type){case"link":return(0,x.jsx)(w,{item:t});case"category":return(0,x.jsx)(y,{item:t});default:throw new Error(`unknown item type ${JSON.stringify(t)}`)}}function C(e){let{className:t}=e;const n=(0,i.$S)();return(0,x.jsx)(k,{items:n.items,className:t})}function k(e){const{items:t,className:n}=e;if(!t)return(0,x.jsx)(C,{...e});const r=(0,i.d1)(t);return(0,x.jsx)("section",{className:(0,o.A)("row",n),children:r.map(((e,t)=>(0,x.jsx)("article",{className:"col col--6 margin-bottom--lg",children:(0,x.jsx)(_,{item:e})},t)))})}},28453:(e,t,n)=>{n.d(t,{R:()=>s,x:()=>l});var r=n(96540);const o={},i=r.createContext(o);function s(e){const t=r.useContext(i);return r.useMemo((function(){return"function"==typeof e?e(t):{...t,...e}}),[t,e])}function l(e){let t;return t=e.disableParentContext?"function"==typeof e.components?e.components(o):e.components||o:s(e.components),r.createElement(i.Provider,{value:t},e.children)}}}]);