"use strict";(self.webpackChunkprocedural_dungeon_wiki=self.webpackChunkprocedural_dungeon_wiki||[]).push([[3453],{83182:(e,o,r)=>{r.r(o),r.d(o,{assets:()=>c,contentTitle:()=>u,default:()=>p,frontMatter:()=>l,metadata:()=>i,toc:()=>d});var n=r(74848),t=r(28453),a=r(11470),s=r(19365);const l={slug:"/guides/Choose-Door",pagination_prev:"guides/Getting-Started/Generating-Dungeon/Is-Valid-Dungeon",pagination_next:"guides/Getting-Started/Packaging"},u="Choose Door",i={id:"guides/Getting-Started/Generating-Dungeon/Choose-Door",title:"Choose Door",description:"This function lets you choose a door actor to spawn between two rooms.\\",source:"@site/docs/guides/Getting-Started/Generating-Dungeon/Choose-Door.md",sourceDirName:"guides/Getting-Started/Generating-Dungeon",slug:"/guides/Choose-Door",permalink:"/ProceduralDungeon/guides/Choose-Door",draft:!1,unlisted:!1,tags:[],version:"current",frontMatter:{slug:"/guides/Choose-Door",pagination_prev:"guides/Getting-Started/Generating-Dungeon/Is-Valid-Dungeon",pagination_next:"guides/Getting-Started/Packaging"},sidebar:"homeSidebar",previous:{title:"Is Valid Dungeon",permalink:"/ProceduralDungeon/guides/Is-Valid-Dungeon"},next:{title:"Packaging your game with the plugin",permalink:"/ProceduralDungeon/guides/Packaging"}},c={},d=[];function h(e){const o={a:"a",br:"br",code:"code",h1:"h1",header:"header",img:"img",p:"p",pre:"pre",...(0,t.R)(),...e.components};return(0,n.jsxs)(n.Fragment,{children:[(0,n.jsx)(o.header,{children:(0,n.jsx)(o.h1,{id:"choose-door",children:"Choose Door"})}),"\n","\n",(0,n.jsxs)(o.p,{children:["This function lets you choose a ",(0,n.jsx)(o.a,{href:"/ProceduralDungeon/guides/Door",children:"door actor"})," to spawn between two rooms.",(0,n.jsx)(o.br,{}),"\n","If you don't want a door between the rooms you just have to return ",(0,n.jsx)(o.code,{children:"none"}),"."]}),"\n",(0,n.jsxs)(o.p,{children:["You should use the ",(0,n.jsx)(o.a,{href:"/ProceduralDungeon/guides/Door-Types",children:(0,n.jsx)(o.code,{children:"Door Type"})})," input to return a door of the correct type if you use multiple door types in your project.",(0,n.jsx)(o.br,{}),"\n","You can check the door type of a door actor class with the function ",(0,n.jsx)(o.code,{children:"Is Door Of Type"}),"."]}),"\n",(0,n.jsxs)(o.p,{children:["You can use the ",(0,n.jsx)(o.code,{children:"Flipped"})," output to control which room the door faces.",(0,n.jsx)(o.br,{}),"\n","If false (default) the door forward will face the ",(0,n.jsx)(o.code,{children:"Current Room"}),".",(0,n.jsx)(o.br,{}),"\n","If true, the door forward will face the ",(0,n.jsx)(o.code,{children:"Next Room"}),"."]}),"\n",(0,n.jsx)(o.p,{children:'One of the room input can be null if there is no room connected to it.\r\nYou certainly want to set an always closed door, or a door actor that act as a "wall".'}),"\n",(0,n.jsx)(o.p,{children:"An example to spawn a simple door if both rooms are valid, or a wall if one of them is not valid (assuming there is only one door type):"}),"\n",(0,n.jsxs)(a.A,{children:[(0,n.jsx)(s.A,{value:"bp",label:"Blueprint",default:!0,children:(0,n.jsx)(o.p,{children:(0,n.jsx)(o.img,{src:r(67638).A+"",width:"917",height:"473"})})}),(0,n.jsxs)(s.A,{value:"cpp",label:"C++",children:[(0,n.jsx)(o.pre,{children:(0,n.jsx)(o.code,{className:"language-cpp",metastring:'title="MyDungeonGenerator.h"',children:'UCLASS()\r\nclass AMyDungeonGenerator : public ADungeonGenerator\r\n{\r\n    GENERATED_BODY()\r\npublic:\r\n    // ...\r\n\r\n    virtual TSubclassOf<ADoor> ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped) override;\r\n\r\n    // The following variables are just for example purpose\r\n    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Dungeon")\r\n    TSubclassOf<ADoor> NormalDoor {nullptr};\r\n    \r\n    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Dungeon")\r\n    TSubclassOf<ADoor> Wall {nullptr};\r\n\r\n    // ...\r\n}\n'})}),(0,n.jsx)(o.pre,{children:(0,n.jsx)(o.code,{className:"language-cpp",metastring:'title="MyDungeonGenerator.cpp"',children:"\r\nTSubclassOf<ADoor> AMyDungeonGenerator::ChooseDoor_Implementation(const URoomData* CurrentRoom, const URoomData* NextRoom, const UDoorType* DoorType, bool& Flipped)\r\n{\r\n    const bool bAreBothRoomValid = IsValid(CurrentRoom) && IsValid(NextRoom);\r\n    return (bAreBothRoomValid) ? NormalDoor : Wall;\r\n}\r\n\n"})})]})]})]})}function p(e={}){const{wrapper:o}={...(0,t.R)(),...e.components};return o?(0,n.jsx)(o,{...e,children:(0,n.jsx)(h,{...e})}):h(e)}},19365:(e,o,r)=>{r.d(o,{A:()=>s});r(96540);var n=r(34164);const t={tabItem:"tabItem_Ymn6"};var a=r(74848);function s(e){let{children:o,hidden:r,className:s}=e;return(0,a.jsx)("div",{role:"tabpanel",className:(0,n.A)(t.tabItem,s),hidden:r,children:o})}},11470:(e,o,r)=>{r.d(o,{A:()=>w});var n=r(96540),t=r(34164),a=r(23104),s=r(56347),l=r(205),u=r(57485),i=r(31682),c=r(70679);function d(e){return n.Children.toArray(e).filter((e=>"\n"!==e)).map((e=>{if(!e||(0,n.isValidElement)(e)&&function(e){const{props:o}=e;return!!o&&"object"==typeof o&&"value"in o}(e))return e;throw new Error(`Docusaurus error: Bad <Tabs> child <${"string"==typeof e.type?e.type:e.type.name}>: all children of the <Tabs> component should be <TabItem>, and every <TabItem> should have a unique "value" prop.`)}))?.filter(Boolean)??[]}function h(e){const{values:o,children:r}=e;return(0,n.useMemo)((()=>{const e=o??function(e){return d(e).map((e=>{let{props:{value:o,label:r,attributes:n,default:t}}=e;return{value:o,label:r,attributes:n,default:t}}))}(r);return function(e){const o=(0,i.XI)(e,((e,o)=>e.value===o.value));if(o.length>0)throw new Error(`Docusaurus error: Duplicate values "${o.map((e=>e.value)).join(", ")}" found in <Tabs>. Every value needs to be unique.`)}(e),e}),[o,r])}function p(e){let{value:o,tabValues:r}=e;return r.some((e=>e.value===o))}function g(e){let{queryString:o=!1,groupId:r}=e;const t=(0,s.W6)(),a=function(e){let{queryString:o=!1,groupId:r}=e;if("string"==typeof o)return o;if(!1===o)return null;if(!0===o&&!r)throw new Error('Docusaurus error: The <Tabs> component groupId prop is required if queryString=true, because this value is used as the search param name. You can also provide an explicit value such as queryString="my-search-param".');return r??null}({queryString:o,groupId:r});return[(0,u.aZ)(a),(0,n.useCallback)((e=>{if(!a)return;const o=new URLSearchParams(t.location.search);o.set(a,e),t.replace({...t.location,search:o.toString()})}),[a,t])]}function m(e){const{defaultValue:o,queryString:r=!1,groupId:t}=e,a=h(e),[s,u]=(0,n.useState)((()=>function(e){let{defaultValue:o,tabValues:r}=e;if(0===r.length)throw new Error("Docusaurus error: the <Tabs> component requires at least one <TabItem> children component");if(o){if(!p({value:o,tabValues:r}))throw new Error(`Docusaurus error: The <Tabs> has a defaultValue "${o}" but none of its children has the corresponding value. Available values are: ${r.map((e=>e.value)).join(", ")}. If you intend to show no default tab, use defaultValue={null} instead.`);return o}const n=r.find((e=>e.default))??r[0];if(!n)throw new Error("Unexpected error: 0 tabValues");return n.value}({defaultValue:o,tabValues:a}))),[i,d]=g({queryString:r,groupId:t}),[m,f]=function(e){let{groupId:o}=e;const r=function(e){return e?`docusaurus.tab.${e}`:null}(o),[t,a]=(0,c.Dv)(r);return[t,(0,n.useCallback)((e=>{r&&a.set(e)}),[r,a])]}({groupId:t}),b=(()=>{const e=i??m;return p({value:e,tabValues:a})?e:null})();(0,l.A)((()=>{b&&u(b)}),[b]);return{selectedValue:s,selectValue:(0,n.useCallback)((e=>{if(!p({value:e,tabValues:a}))throw new Error(`Can't select invalid tab value=${e}`);u(e),d(e),f(e)}),[d,f,a]),tabValues:a}}var f=r(92303);const b={tabList:"tabList__CuJ",tabItem:"tabItem_LNqP"};var D=r(74848);function x(e){let{className:o,block:r,selectedValue:n,selectValue:s,tabValues:l}=e;const u=[],{blockElementScrollPositionUntilNextRender:i}=(0,a.a_)(),c=e=>{const o=e.currentTarget,r=u.indexOf(o),t=l[r].value;t!==n&&(i(o),s(t))},d=e=>{let o=null;switch(e.key){case"Enter":c(e);break;case"ArrowRight":{const r=u.indexOf(e.currentTarget)+1;o=u[r]??u[0];break}case"ArrowLeft":{const r=u.indexOf(e.currentTarget)-1;o=u[r]??u[u.length-1];break}}o?.focus()};return(0,D.jsx)("ul",{role:"tablist","aria-orientation":"horizontal",className:(0,t.A)("tabs",{"tabs--block":r},o),children:l.map((e=>{let{value:o,label:r,attributes:a}=e;return(0,D.jsx)("li",{role:"tab",tabIndex:n===o?0:-1,"aria-selected":n===o,ref:e=>u.push(e),onKeyDown:d,onClick:c,...a,className:(0,t.A)("tabs__item",b.tabItem,a?.className,{"tabs__item--active":n===o}),children:r??o},o)}))})}function v(e){let{lazy:o,children:r,selectedValue:a}=e;const s=(Array.isArray(r)?r:[r]).filter(Boolean);if(o){const e=s.find((e=>e.props.value===a));return e?(0,n.cloneElement)(e,{className:(0,t.A)("margin-top--md",e.props.className)}):null}return(0,D.jsx)("div",{className:"margin-top--md",children:s.map(((e,o)=>(0,n.cloneElement)(e,{key:o,hidden:e.props.value!==a})))})}function y(e){const o=m(e);return(0,D.jsxs)("div",{className:(0,t.A)("tabs-container",b.tabList),children:[(0,D.jsx)(x,{...o,...e}),(0,D.jsx)(v,{...o,...e})]})}function w(e){const o=(0,f.A)();return(0,D.jsx)(y,{...e,children:d(e.children)},String(o))}},67638:(e,o,r)=>{r.d(o,{A:()=>n});const n=r.p+"assets/images/ChooseDoor-7fc7c159a668a760be43c01f3166a200.jpg"},28453:(e,o,r)=>{r.d(o,{R:()=>s,x:()=>l});var n=r(96540);const t={},a=n.createContext(t);function s(e){const o=n.useContext(a);return n.useMemo((function(){return"function"==typeof e?e(o):{...o,...e}}),[o,e])}function l(e){let o;return o=e.disableParentContext?"function"==typeof e.components?e.components(t):e.components||t:s(e.components),n.createElement(a.Provider,{value:o},e.children)}}}]);