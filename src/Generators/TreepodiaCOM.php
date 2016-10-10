<?php

namespace ElasticExport\Generators;

use Plenty\Modules\DataExchange\Contracts\XMLGenerator;
use Plenty\Modules\Helper\Services\ArrayHelper;
use Plenty\Modules\Helper\Models\KeyValue;
use Plenty\Modules\Item\DataLayer\Models\Record;
use Plenty\Modules\Item\DataLayer\Models\RecordList;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use ElasticExport\Helper\ElasticExportHelper;
use Plenty\Modules\Category\Contracts\CategoryBranchRepositoryContract;
use Plenty\Modules\Category\Contracts\CategoryRepositoryContract;
use Plenty\Modules\Item\Manufacturer\Contracts\ManufacturerRepositoryContract;
use Plenty\Modules\Item\Manufacturer\Models\Manufacturer;
use Plenty\Modules\Category\Models\Category;

class TreepodiaCOM extends XMLGenerator
{
	/**
	 * @var string
	 */
	protected $version = '1.0';

	/**
	 * @var string
	 */
	protected $encoding = 'UTF-8';

	/**
	 * @var bool
	 */
	protected $formatOutput = true;

	/**
	 * @var bool
	 */
	protected $preserveWhiteSpace = true;

	/*
     * @var ElasticExportHelper
     */
    private $elasticExportHelper;

	/*
	 * @var ArrayHelper
	 */
	private $arrayHelper;

	/**
     * CategoryBranchRepositoryContract $categoryBranchRepository
     */
    private $categoryBranchRepository;

	/**
     * CategoryRepositoryContract $categoryRepository
     */
    private $categoryRepository;

    /**
     * ManufacturerRepositoryContract $manufacturerRepository
     */
    private $manufacturerRepository;

	/**
     * TreepodiaDE constructor.
     * @param ElasticExportHelper $elasticExportHelper
     * @param ArrayHelper $arrayHelper
     * @param CategoryBranchRepositoryContract $categoryBranchRepository
     * @param CategoryRepositoryContract $categoryRepository
     * @param ManufacturerRepositoryContract $manufacturerRepository
     */
    public function __construct(
    	ElasticExportHelper $elasticExportHelper,
    	ArrayHelper $arrayHelper,
    	CategoryBranchRepositoryContract $categoryBranchRepository,
    	CategoryRepositoryContract $categoryRepository,
    	ManufacturerRepositoryContract $manufacturerRepository
    )
    {
        $this->elasticExportHelper = $elasticExportHelper;
		$this->arrayHelper = $arrayHelper;
		$this->categoryBranchRepository = $categoryBranchRepository;
		$this->categoryRepository = $categoryRepository;
		$this->manufacturerRepository = $manufacturerRepository;

		$this->init('products');
    }

	/**
	 * @param mixed $resultData
	 * @param array $formatSettings
	 */
	protected function generateContent(mixed $resultData, array $formatSettings = [])
	{
		if($resultData instanceof RecordList)
		{
			$settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

			foreach($resultData as $item)
			{
				$product = $this->createElement('product');
				$this->root()->appendChild($product);

				// sku
				$product->appendChild($this->createElement('sku', $item->itemBase->id));

				// price
				$product->appendChild($this->createElement('price', number_format($this->elasticExportHelper->getPrice($item), 2)));

				// name
				$product->appendChild($this->createElement('name', $this->elasticExportHelper->getName($item, $settings)));

				// commodity
				$category = $this->getTopLevelCategory($item, $settings);
				if($category instanceof Category)
				{
					foreach($category->details as $detail)
					{
						if($detail->lang == $settings->get('lang'))
						{
							$product->appendChild($this->createElement('commodity', $detail->name));
						}
					}
				}


				// description
				$product->appendChild($description = $this->createElement('description'));
				$description->appendChild($this->createCDATASection($this->elasticExportHelper->getDescription($item, $settings)));

				// brand-name, brand-logo
				if((int) $item->itemBase->producerId > 0)
				{
					$manufacturer = $this->getProducer($item->itemBase->producerId);

					if($manufacturer instanceof Manufacturer)
					{
						if(strlen($manufacturer->externalName) > 0)
						{
							$product->appendChild($brandName = $this->createElement('brand-name'));
							$brandName->appendChild($this->createCDATASection($manufacturer->externalName));
						}
						elseif(strlen($manufacturer->name) > 0)
						{
							$product->appendChild($brandName = $this->createElement('brand-name'));
							$brandName->appendChild($this->createCDATASection($manufacturer->name));
						}

						if(strlen($manufacturer->logo) > 0)
						{
							$product->appendChild($this->createElement('brand_logo', $manufacturer->logo));
						}
					}
				}

				// page-url
				$product->appendChild($pageUrl = $this->createElement('page-url'));
				$pageUrl->appendChild($this->createCDATASection($this->elasticExportHelper->getUrl($item, $settings, false)));

				// image-url
				foreach($this->elasticExportHelper->getImageList($item, $settings) as $image)
				{
					$product->appendChild($this->createElement('image-url', $image));
				}

				// catch-phrase
				foreach($this->getCatchPhraseList($item) as $catchPhrase)
				{
					$product->appendChild($this->createElement('catch-phrase', htmlspecialchars($catchPhrase)));
				}

				// free-shipping
				if($this->elasticExportHelper->getShippingCost($item, $settings) <= 0.00)
				{
					$product->appendChild($this->createElement('free-shipping', 1));
				}

				// youtubetag, Video-Sitemaptag
				foreach($this->getKeywords($item, $settings) as $keyword)
				{
					$product->appendChild($this->createElement('youtubetag', htmlspecialchars(trim($keyword))));
					$product->appendChild($this->createElement('Video-Sitemaptag', htmlspecialchars(trim($keyword))));
				}
			}

			$this->build();
		}
	}

	/**
	 * Get the top level category.
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return Category
	 */
	public function getTopLevelCategory(Record $item, KeyValue $settings):Category
	{
		$lang = $settings->get('lang') ? $settings->get('lang') : 'de';

		$categoryBranch = $this->categoryBranchRepository->find($item->variationStandardCategory->categoryId);

		if(!is_null($categoryBranch))
		{
			$list = [
				$categoryBranch->category6Id,
				$categoryBranch->category5Id,
				$categoryBranch->category4Id,
				$categoryBranch->category3Id,
				$categoryBranch->category2Id,
				$categoryBranch->category1Id
			];

			$categoryList = [];

			foreach($list AS $category)
			{
				if($category > 0)
				{
					$categoryList[] = $category;
				}
			}

			$categoryId = $categoryList[0];

			return $this->categoryRepository->get((int) $categoryId, $lang);
		}

		return null;
	}

	/**
	 * Get producer.
	 * @param int $manufacturerId
	 * @return Manufacturer
	 */
	public function getProducer(int $manufacturerId):mixed
	{
		return $this->manufacturerRepository->findById($manufacturerId);
	}

	/**
	 * Get catch phrase list.
	 * @param Record $item
	 * @return array<string>
	 */
	private function getCatchPhraseList(Record $item):array										// TODO
	{
		$list = [
			$item->itemBase->free1,
			$item->itemBase->free2,
			$item->itemBase->free3,
			$item->itemBase->free4,
		];

		$filteredList = [];

		foreach($list AS $value)
		{
			if(strlen($value) > 0)
			{
				$filteredList[] = $value;
			}
		}

		return $filteredList;
//				->filter(($free) ==> strlen($free) > 0)
//				->toArray();
	}

	/**
	 * Get keywords.
	 * @param Record $item
	 * @param KeyValue $settings
	 * @return array<string>
	 */
	public function getKeywords(Record $item, KeyValue $settings):array
	{
		$list = explode(',', $item->itemDescription->keywords);

		$category = $this->getTopLevelCategory($item, $settings);
		if($category instanceof Category)
		{
			foreach($category->details as $detail)
			{
				if($detail->lang == $settings->get('lang'))
				{
					$list = array_merge($list, explode(',', $detail->metaKeywords));
				}
			}
		}

		return $list;
	}
}
